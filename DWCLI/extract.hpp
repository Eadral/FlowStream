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
		float contrast = 8.f) : contrast(contrast) {
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

		Mat QrRoi = extracted(cv::Rect(0, 0, extracted.cols*INSERT_SCALE, extracted.rows*INSERT_SCALE));
		Mat extractedQR;
		QrRoi.copyTo(extractedQR);
		
		
		//imshow("Qr", extractedQR);

		Mat Qrout;
		extractedQR.copyTo(Qrout);
		Qrout = Qrout.mul(255);
		Qrout.convertTo(Qrout, CV_8U);
		//imshow("Qr", Qrout);

		QREnhance(Qrout);

		imwrite(savename, Qrout);

		//waitKey(0);
	}

	void QREnhance(Mat &img) {
		equalizeHist(img, img);
		img.convertTo(img, -1, contrast, 0);

		//Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0, 5, 0, 0, -1, 0);
		//filter2D(img, img, CV_8U, kernel);
	}

	float contrast;
	int channel;
};