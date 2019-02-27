#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "parameters.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;
using namespace Meow;

class Sign {
public:

	Sign(string colorSpace, vector<string> channelNames,
		float power = 1.3f, float bright = 0.14f, float mixed_factor = 0.8f)
		: power(power), bright(bright), mixed_factor(mixed_factor)
	{
		if (colorSpace == "RGB") {
			for (string channelName : channelNames) {
				if (channelName == "b") channelAts.push_back(0);
				if (channelName == "g") channelAts.push_back(1);
				if (channelName == "r") channelAts.push_back(2);
			}
		} else {
			throw "Unsupported color space. -h for help.";
		}
	}

	void sign(string ori_filename, string sign_filename, string save_filename) {

		Mat ori = imread(ori_filename);
		//imshow("ori", ori);
		Mat sign = imread(sign_filename, IMREAD_GRAYSCALE);

		vector<Mat> channels;
		split(ori, channels);

		for (int channel : channelAts) {
			channels[channel] = signChannel(channels[channel], sign);
		}

		Mat signedImage;
		merge(channels, signedImage);

		imwrite(save_filename, signedImage);

	}


private:

	float power;
	float bright;
	float mixed_factor;
	vector<int> channelAts;


	Mat signChannel(Mat oriChannel, Mat sign) {
		//Mat show;

		//show = oriChannel;
		//imshow("ori", show);

		Mat oriComplex;
		img_dft(oriChannel, oriComplex);

		shift(oriComplex);
		
		//show = show_mag(oriComplex);
		//imshow("dft", show);

		Mat signedComplex;
		signImage(oriComplex, sign, signedComplex);

		//show = show_mag(signedComplex);
		//imshow("signed_dft", show);

		Mat signed_idftF = img_idft(signedComplex);

		enhance(signed_idftF);

		//show = show_mag(signed_idftF);
		//imshow("signed", show);

		Mat signedChannel;
		signed_idftF.copyTo(signedChannel);
		signedChannel = signedChannel.mul(255);
		signedChannel.convertTo(signedChannel, CV_8U);

		//waitKey(0);

		return signedChannel;
	}

	void enhance(Mat &img) {
		img.convertTo(img, CV_32F, 1.0, bright);

	}


	void signImage(Mat complex, Mat signIm, Mat &out) {
		Mat planes[] = { Mat::zeros(complex.size(),CV_32F), Mat::zeros(complex.size(),CV_32F) };
		split(complex, planes);

		Mat signI = Mat::zeros(planes[0].rows*INSERT_SCALE, planes[0].cols*INSERT_SCALE, signI.type());
		resize(signIm, signI, signI.size());

		Mat signF = Mat::zeros(signI.size(), CV_32F);
		signI.convertTo(signF, CV_32F);

		cv::pow(signF, power, signF);

		Mat plane0Roi = planes[0](cv::Rect(0, 0, signF.cols, signF.rows));
		addWeighted(plane0Roi, 1 - mixed_factor, signF, mixed_factor, 0., plane0Roi);

		Mat plane1Roi = planes[1](cv::Rect(0, 0, signF.cols, signF.rows));
		addWeighted(plane1Roi, 1 - mixed_factor, signF, mixed_factor, 0., plane1Roi);

		flip(signF, signF, 0);
		flip(signF, signF, 1);

		plane0Roi = planes[0](cv::Rect(planes[0].cols - signF.cols, planes[0].rows - signF.rows, signF.cols, signF.rows));
		addWeighted(plane0Roi, 1 - mixed_factor, signF, mixed_factor, 0., plane0Roi);

		plane1Roi = planes[1](cv::Rect(planes[0].cols - signF.cols, planes[0].rows - signF.rows, signF.cols, signF.rows));
		addWeighted(plane1Roi, 1 - mixed_factor, signF, mixed_factor, 0., plane1Roi);

		merge(planes, 2, out);
	}



};

