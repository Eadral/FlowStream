#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

void shift_single(Mat ori) {
	int cx = ori.cols / 2;
	int cy = ori.rows / 2;

	Mat q0(ori, Rect(0, 0, cx, cy));      
	Mat q1(ori, Rect(cx, 0, cx, cy));     
	Mat q2(ori, Rect(0, cy, cx, cy));     
	Mat q3(ori, Rect(cx, cy, cx, cy));    

	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

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


void img_dft(cv::Mat &img, cv::Mat &complexI)
{
	Mat padded;
	int m = getOptimalDFTSize(img.rows);
	int n = getOptimalDFTSize(img.cols);

	copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(),CV_32F) };
	merge(planes, 2, complexI);

	dft(complexI, complexI);
}


void sign(Mat &complex, Mat signIm, Mat &out) {
	Mat planes[] = { Mat::zeros(complex.size(),CV_32F), Mat::zeros(complex.size(),CV_32F) };
	split(complex, planes);

	Mat signI = Mat::zeros(planes[0].rows*0.25, planes[0].cols*0.25, signI.type());
	resize(signIm, signI, signI.size());

	Mat signF = Mat::zeros(signI.size(), CV_32F);
	signI.convertTo(signF, CV_32F);

	cv::pow(signF, 1.5, signF);

	Mat plane0Roi = planes[0](Rect(0, 0, signF.cols, signF.rows));
	addWeighted(plane0Roi, 0., signF, 1., 0., plane0Roi);
	
	Mat plane1Roi = planes[1](Rect(0, 0, signF.cols, signF.rows));
	addWeighted(plane1Roi, 0., signF, 1., 0., plane1Roi);

	flip(signF, signF, 0);
	flip(signF, signF, 1);

	plane0Roi = planes[0](Rect(planes[0].cols - signF.cols, planes[0].rows - signF.rows, signF.cols, signF.rows));
	addWeighted(plane0Roi, 0., signF, 1., 0., plane0Roi);

	plane1Roi = planes[1](Rect(planes[0].cols - signF.cols, planes[0].rows - signF.rows, signF.cols, signF.rows));
	addWeighted(plane1Roi, 0., signF, 1., 0., plane1Roi);

	merge(planes, 2, out);
}