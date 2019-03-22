#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "utility.hpp"
#include "parameters.hpp"

using namespace std;
using namespace cv;
using namespace Meow;

class Extract {
public:

	Extract(string extract_colorSpace, string extract_channel, 
		float contrast, bool debug) 
		: contrast(contrast), debug(debug) {
		if (extract_colorSpace == "RGB") {
			if (extract_channel == "b") channel = 0;
			if (extract_channel == "g") channel = 1;
			if (extract_channel == "r") channel = 2;
		} else {
			throw "Unsupported color space. -h for help.";
		}
	}

	void extractToQR(string filename, string savename) {

		Mat img = imread(filename);
		//imshow("img", img);
		vector<Mat> channels;
		split(img, channels);
		Mat selected = channels[channel];

		Mat complex;
		img_dft(selected, complex);
		shift(complex);
		Mat extracted = show_mag(complex);
		//imshow("QrRoi", extracted);

		auto size = min(extracted.cols*INSERT_SCALE, extracted.rows*INSERT_SCALE);

		Mat QrRoi = extracted(cv::Rect(0, 0, size, size));
		Mat extractedQR;
		QrRoi.copyTo(extractedQR);
		
		
		//imshow("Qr", extractedQR);

		Mat Qrout;
		extractedQR.copyTo(Qrout);
		Qrout = Qrout.mul(255);
		Qrout.convertTo(Qrout, CV_8U);
		//imshow("Qr", Qrout);

		QREnhance(Qrout);

		Mat binary;
		binary = threshold(Qrout, binary, 128, 255, THRESH_BINARY);

		imwrite(savename, binary);

		//waitKey(0);
	}

	void extractToQRDCT(string filename, string savename) {

		Mat img = imread(filename);
		// cv::cvtColor(img, img, COLOR_RGB2YUV);
		//imshow("img", img);
		vector<Mat> channels;
		split(img, channels);
		Mat selected = channels[channel];
		// imshow("output", selected);
		selected.convertTo(selected, CV_32F, 1.0/255, 0);

		Mat complex;
		dct(selected, complex);
		// shift(complex);
		if (debug) imshow("dct", complex);
		// waitKey(0);
		Mat extracted = complex;
		// imshow("QrRoi", extracted);

		auto size = min(extracted.cols*INSERT_SCALE, extracted.rows*INSERT_SCALE);

		Mat QrRoi = extracted(cv::Rect(extracted.cols / 2 - size / 2, extracted.rows / 2 - size / 2, size, size));
		Mat extractedQR;
		QrRoi.copyTo(extractedQR);

		
		if (debug) imshow("Qr", extractedQR);

		Mat Qrout;
		extractedQR.copyTo(Qrout);
		Qrout.convertTo(Qrout, CV_8U, 255 * contrast, 0);
		// imshow("Qr", Qrout);

		// QREnhance(Qrout);
		Mat binary;
		threshold(Qrout, binary, 200, 255, THRESH_BINARY | THRESH_TRIANGLE);

		medianBlur(binary, binary, 3);
		threshold(binary, binary, 200, 255, THRESH_BINARY | THRESH_TRIANGLE);
		Mat output;
		resize(binary, output, cv::Size(128, 128));
		if (debug) imshow("output", output);
		imwrite(savename, output);

		if (debug) waitKey(0);
	}

	void QREnhance(Mat &img) {
		//equalizeHist(img, img);
		img.convertTo(img, -1, contrast, 0);

		//Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0, 5, 0, 0, -1, 0);
		//filter2D(img, img, CV_8U, kernel);
	}

	float contrast;
	int channel;
	bool debug = false;
};