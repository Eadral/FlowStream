#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "parameters.hpp"

using namespace std;
using namespace cv;

namespace Meow {

	void shift_single(Mat ori) {
		int cx = ori.cols / 2;
		int cy = ori.rows / 2;

		Mat q0(ori, cv::Rect(0, 0, cx, cy));
		Mat q1(ori, cv::Rect(cx, 0, cx, cy));
		Mat q2(ori, cv::Rect(0, cy, cx, cy));
		Mat q3(ori, cv::Rect(cx, cy, cx, cy));

		Mat tmp;
		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		q1.copyTo(tmp);
		q2.copyTo(q1);
		tmp.copyTo(q2);
	}

	/// <summary> Spin the complex image. </summary>
	/// <param name="complex"> Complex image. </param>
	void shift(Mat &complex) {
		Mat planes[] = { Mat::zeros(complex.size(),CV_32F), Mat::zeros(complex.size(),CV_32F) };
		split(complex, planes);

		shift_single(planes[0]);
		shift_single(planes[1]);

		merge(planes, 2, complex);
	}

	Mat show_mag(Mat complexI) {
		Mat planes[] = { Mat::zeros(complexI.size(),CV_32F), Mat::zeros(complexI.size(),CV_32F) };
		split(complexI, planes);

		Mat magI;
		magnitude(planes[0], planes[1], magI);
		//magI = magI(Rect(0, 0, magI.cols&-2, magI.rows&-2));   // single to even
		magI += Scalar::all(1);
		log(magI, magI);
		normalize(magI, magI, 1, 0, NORM_MINMAX);

		return magI;
	}


	Mat img_idft(Mat complexI) {
		Mat _complexI;
		complexI.copyTo(_complexI);
		Mat iDft[] = { Mat::zeros(_complexI.size(),CV_32F), Mat::zeros(_complexI.size(),CV_32F) };
		idft(_complexI, _complexI);
		split(_complexI, iDft);
		magnitude(iDft[0], iDft[1], iDft[0]);
		normalize(iDft[0], iDft[0], 1, 0, NORM_MINMAX);

		return iDft[0];
	}

	Mat img_idct(Mat img) {
		Mat idct_img;
		idct(img, idct_img);

		return idct_img;
	}


	void img_dft(cv::Mat &img, cv::Mat &complexI) {
		Mat padded;
		int m = getOptimalDFTSize(img.rows);
		int n = getOptimalDFTSize(img.cols);

		copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(),CV_32F) };
		merge(planes, 2, complexI);

		dft(complexI, complexI);
		// dct(complexI, complexI);
	}

	void img_dct(cv::Mat &img, cv::Mat &img_out) {
		// Mat padded;
		// int m = getOptimalDFTSize(img.rows);
		// int n = getOptimalDFTSize(img.cols);
		//
		// copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, BORDER_CONSTANT, Scalar::all(0));

		dct(img, img_out);

	}


}
