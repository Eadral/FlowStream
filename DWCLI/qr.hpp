#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "CLI11.hpp"
//#include "QrCode.hpp"
//#include "Magick++.h"
#include "BarcodeReader.h"
#include "BarcodeGenerator.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

using namespace std;
//using namespace qrcodegen;
//using namespace Magick;
using namespace ZXing;

class QR {
public:

	//static void creat_qr_image(string text, string filename) {
	//	QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::HIGH);

	//	ofstream fout("qrsvg.svg");
	//	fout << qr.toSvgString(0) << endl;
	//	fout.close();

	//	Magick::Image image("qrsvg.svg");
	//	image.magick("png");
	//	image.write(filename);
	//}

	static void creat_qr_image(wstring text, wstring filename, string format = "QR_CODE") {
		BarcodeGenerator gen(format);
		auto qr = gen.generate(text, 128, 128);
		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		qr->Save(filename.c_str(), &pngClsid, NULL);
	}

	static string scan_qr_image(wstring filename) {
		BarcodeReader reader(true, true, "QR_CODE");
		Bitmap qr(filename.c_str());
		auto result = reader.scan(qr);
		return result.text;
	}




	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		ImageCodecInfo* pImageCodecInfo = NULL;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL)
			return -1;  // Failure

		Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

};

