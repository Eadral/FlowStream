#include <iostream>
#include <locale>
#include <codecvt>
#include <string>


#include "CLI11.hpp"
//#include "QrCode.hpp"
//#include "Magick++.h"

#include "qr.hpp"
#include "sign.hpp"
#include "extract.hpp"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

using namespace std;
//using namespace qrcodegen;

int main(int argc, char **argv) {
	//InitializeMagick(*argv);
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	

	CLI::App app{"a command line"};
	app.require_subcommand(1);

#pragma region QR

	auto qr = app.add_subcommand("qr", "QRcode tools");
	
	bool qr_scan;
	qr->add_flag("--scan, -s", qr_scan, "Set to scan, otherwise generate.");

	string qr_text = "Hello Watering!";
	qr->add_option("--text, -t", qr_text, "Content in QRcode");

	string qr_file = "qrcode.png";
	qr->add_option("-o", qr_file, "QRcode image path");

	//int qr_errCorLvl = 3;
	//qr->add_option("--errCorLvl, -e", qr_errCorLvl, "Error correction level. Number: 0-3 (7%, 15%, 25%, 30%).", true);

	string qr_format = "QR_CODE";
	qr->add_option("--format, -f", qr_format, "Supported formats are: "
		"AZTEC, "
		"CODABAR, "
		"CODE_39, "
		"CODE_93, "
		"CODE_128, "
		"DATA_MATRIX, "
		"EAN_8, "
		"EAN_13, "
		"ITF, "
		"PDF_417, "
		"QR_CODE, "
		"UPC_A, "
		"UPC_E. ", true);

#pragma endregion


#pragma region Sign

	auto sign = app.add_subcommand("sign", "Use an image to sign an image. (Add watermark)");

	string sign_colorSpace = "RGB";
	sign->add_option("--color_space, -s", sign_colorSpace, "Choose the color space. (RGB) ", true);

	vector<string> sign_channels;
	sign->add_option("--channels, -c", sign_channels, "Channels to be signed.");

	float sign_power = 1.4f;
	sign->add_option("--power, -p", sign_power, "Power of signature.", true);

	//float sign_bright = 0.14f;
	//sign->add_option("--bright, -b", sign_bright, "Add fixed brightness to the channel.", true);

	float fixed_factor = 0.8f;
	sign->add_option("--fixed_factor, -f", fixed_factor, "Percent of after fix. (0-1)", true);

	float sign_mixed_factor = 0.8f;  
	sign->add_option("--mixfactor, -m", sign_mixed_factor, "Choose the mixed_factor of the signing(感觉这么理解没问题吧)", true);

	string str_origin;
	sign->add_option("--origin,-o", str_origin, "Get the name of original image");

	string str_water;
	sign->add_option("--water, -w", str_water, "Get the name of the image with watermark");

	string str_saveFileName;
	sign->add_option("--result, -r", str_saveFileName, "Set the name of the filename which  saves the image added watermark");
	
	
#pragma endregion


#pragma region Extract

	auto extract = app.add_subcommand("extract", "Extract the signature in an image.");

	//string extract;
	//extract->add_option("--extract, -e", extract, "Extract the signature in an image.");

	string filename;
	extract->add_option("--filename, -f", filename, "The filename of the image to be extracted");

	string savename;
	extract->add_option("--output, -o", savename, "The savename of the image extracted");
	
	string extract_colorSpace = "RGB";
	extract->add_option("--color_space, -s", extract_colorSpace, "Choose the color space. (RGB) ", true);

	string extract_channel;
	extract->add_option("--channel, -n", extract_channel, "Channel to extract.");

	float extract_contrast = 1.f;
	extract->add_option("--contrast, -c", extract_contrast, "Set the extract constrast", true);

#pragma endregion


#pragma region Video

	auto video = app.add_subcommand("video", "Operation about video");

	bool video_sign = false;
	video->add_flag("--sign, -s", video_sign, "Sign the video");

	string video_sign_filename;
	video->add_option("--image, -i", video_sign_filename, "Image");

	string video_filename;
	video->add_option("--filename, -f", video_filename, "Path of the video.");

	string video_output_filename = "output.png";
	video->add_option("--output, -o", video_output_filename, "Output filename");

#pragma endregion



	CLI11_PARSE(app, argc, argv);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	if (qr->parsed()) {
		if (qr_scan) {
			cout << QR::scan_qr_image(converter.from_bytes(qr_file)) << endl;
		} else {
			QR::creat_qr_image(converter.from_bytes(qr_text), converter.from_bytes(qr_file), qr_format);
			//QR::creat_qr_image(qr_text, qr_file);
		}
		
	}

	if (sign->parsed()) {
		try {
			Sign sign_img = Sign(sign_colorSpace, sign_channels,
				sign_power, sign_mixed_factor, fixed_factor);

			sign_img.sign(str_origin, str_water, str_saveFileName);
		} catch (string error) {
			cout << error << endl;
			return -1;
		}

	}

	if (extract->parsed()) {
		try {
			Extract extract_tool(extract_colorSpace, extract_channel, extract_contrast);
			extract_tool.extractToQRDCT(filename, savename);
		} catch (string error) {
			cout << error << endl;
			return -1;
		}
		
	}

	if (video->parsed()) {
		if (video_sign) {
			Sign sign_img = Sign("RGB", vector<string>{"r", "g", "b"},
				1.4, 1.0, 0.6);

			auto cap = VideoCapture(video_filename);
			auto avi = VideoWriter(video_output_filename, FOURCC("MP4V"), cap.get(CAP_PROP_FPS),
				cv::Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_WIDTH)));

			Mat frame;
			Mat signed_frame;

			int length = cap.get(CAP_PROP_FRAME_COUNT);
			for (int i = 0; i < length; i++) {
				cap.read(frame);
				signed_frame = sign_img.sign_mat(frame, video_sign_filename);
				printf("Please wait...(%d/%d)\n", i+1, length);
				//imshow("test", signed_frame);
				//waitKey(0);
				avi.write(signed_frame);
			}
			
			avi.release();

		} else {
			auto cap = VideoCapture(video_filename);
			Mat frame;
			cap.read(frame);

			imwrite(video_output_filename, frame);
		}
	}

	return 0;
}