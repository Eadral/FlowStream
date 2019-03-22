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
		float power, float mixed_factor, float fixed_factor)
		: power(power), mixed_factor(mixed_factor), fixed_factor(fixed_factor)
	{
		if (colorSpace == "RGB") {
			for (string channelName : channelNames) {
				if (channelName == "b") channelAts.push_back(0);
				if (channelName == "g") channelAts.push_back(1);
				if (channelName == "r") channelAts.push_back(2);
			}
		} else if (colorSpace == "YUV") {
			for (string channelName : channelNames) {
				if (channelName == "y") channelAts.push_back(0);
				if (channelName == "u") channelAts.push_back(1);
				if (channelName == "v") channelAts.push_back(2);
			}
		} else {
			throw "Unsupported color space. -h for help.";
		}
	}

	Mat sign_mat(Mat ori, string sign_filename) {
		auto oriSize = ori.size();

		int m = getOptimalDFTSize(ori.rows);
		int n = getOptimalDFTSize(ori.cols);

		//resize(ori, ori, cv::Size(n, m));

		//imshow("ori", ori);
		Mat sign = imread(sign_filename, IMREAD_GRAYSCALE);

		vector<Mat> channels;
		split(ori, channels);

		for (int channel : channelAts) {
			channels[channel] = signChannel(channels[channel], sign);
		}

		Mat signedImage;
		merge(channels, signedImage);

		Mat signedImageRecover = signedImage(cv::Rect(0, 0, ori.cols, ori.rows));

		return signedImageRecover;
	}

	void sign(string ori_filename, string sign_filename, string save_filename) {
		
		int channel = channelAts[0];

		Mat ori = imread(ori_filename);
		// cv::cvtColor(ori, ori, COLOR_RGB2YUV);

		auto oriSize = ori.size();

		int m = getOptimalDFTSize(ori.rows);
		int n = getOptimalDFTSize(ori.cols);

		//resize(ori, ori, cv::Size(n, m));

		//imshow("ori", ori);
		Mat sign = imread(sign_filename, IMREAD_GRAYSCALE);

		vector<Mat> channels;
		split(ori, channels);

		for (int channel : channelAts) {
			channels[channel] = signChannelDCT(channels[channel], sign);
		}

		Mat signedImage;
		// imshow("before", channels[channel]);
		merge(channels, signedImage);

		Mat signedImageRecover = signedImage;
		// cv::cvtColor(signedImageRecover, signedImageRecover, COLOR_YUV2RGB);
		imwrite(save_filename, signedImageRecover);

		// imshow("signedImage", signedImageRecover);

		Mat again = imread(save_filename);
		// imshow("again", again);
		// cv::cvtColor(again, again, COLOR_RGB2YUV);
		// imshow("return", again);
		vector<Mat> channelsA;
		split(again, channelsA);

		// imshow("again", channelsA[channel]);

		// waitKey(0);
	}


private:

	float power;
	float bright;
	float mixed_factor;
	float fixed_factor;
	vector<int> channelAts;
	bool isDct = false;

	Mat signChannel(Mat oriChannel, Mat sign) {

#ifdef SHOW
		Mat show;

		show = oriChannel;
		// cv::imshow("ori", show);
#endif // SHOW


		Mat oriComplex;
		img_dft(oriChannel, oriComplex);

		shift(oriComplex);
		
#ifdef SHOW
		show = show_mag(oriComplex);
		// imshow("dft", show);
#endif // SHOW


		Mat signedComplex;
		signImage(oriComplex, sign, signedComplex);

#ifdef SHOW
		show = show_mag(signedComplex);
		// imshow("signed_dft", show);
#endif // SHOW


		Mat signed_idftF = img_idft(signedComplex);

		//enhance(signed_idftF);


#ifdef SHOW
		show = show_mag(signed_idftF);
		// imshow("signed", show);
#endif // SHOW


		Mat signedChannel;
		signed_idftF.copyTo(signedChannel);
		signedChannel = signedChannel.mul(255);
		signedChannel.convertTo(signedChannel, CV_8U);

		Mat h_matched = histogram_Matching(signedChannel, oriChannel);

		Mat final_result;

		addWeighted(signedChannel, fixed_factor, h_matched, fixed_factor, 0, final_result);

		// waitKey(0);

		return final_result;
	}

	Mat signChannelDCT(Mat oriChannel, Mat sign) {
		

		Mat orif;
		oriChannel.convertTo(orif, CV_32F, 1.0/255, 0);

		Mat dct_out;
		dct(orif, dct_out);

		// imshow("origin", orif);
		// imshow("dct", dct_out);

		Mat signed_img;
		signImageDct(dct_out, sign, signed_img);

		// imshow("signed", signed_img);

		Mat signed_idct;
		idct(signed_img, signed_idct);

		// imshow("signed_idct", signed_idct);

		// waitKey(0);

		Mat returned;
		signed_idct.convertTo(returned, CV_8U, 255, 0);

		Mat h_matched = histogram_Matching(returned, oriChannel);

		Mat final_result;

		addWeighted(returned, 1 - fixed_factor, h_matched, fixed_factor, 0, final_result);
		
		// waitKey(0);

		return returned;

	}

	void signImageDct(Mat dctI, Mat signIm, Mat &out) {

		auto size = min(dctI.rows*INSERT_SCALE, dctI.cols*INSERT_SCALE);

		Mat signI = Mat::zeros(size, size, signI.type());
		resize(signIm, signI, signI.size());

		Mat signF = Mat::zeros(signI.size(), CV_32F);
		signI.convertTo(signF, CV_32F);

		cv::pow(signF, power, signF);

		// mixed_factor = 0.0005;

		Mat plane0Roi = dctI(cv::Rect(dctI.cols/2 - signF.cols/2, dctI.rows/2-signF.rows/2, signF.cols, signF.rows));
		addWeighted(plane0Roi, 1 - mixed_factor, signF, mixed_factor, 0., plane0Roi);

		out = dctI;

	}

	void enhance(Mat &img) {
		img.convertTo(img, CV_32F, 1.0, bright);

	}

	Mat histogram_Matching(Mat matSrc, Mat matDst) {

		//Mat srcBGR[3];
		//Mat dstBGR[3];
		//Mat retBGR[3];
		//split(matSrc, srcBGR);
		//split(matDst, dstBGR);

		//histMatch_Value(srcBGR[0], dstBGR[0], retBGR[0]);
		//histMatch_Value(srcBGR[1], dstBGR[1], retBGR[1]);
		//histMatch_Value(srcBGR[2], dstBGR[2], retBGR[2]);



		Mat matResult;
		//merge(retBGR, 3, matResult);

		histMatch_Value(matSrc, matDst, matResult);

		//imshow("src", matSrc);
		//imshow("dst", matDst);
		//imshow("Ret", matResult);

		//waitKey(0);

		return matResult;
	}

	bool histMatch_Value(Mat matSrc, Mat matDst, Mat &matRet) {
		if (matSrc.empty() || matDst.empty() || 1 != matSrc.channels() || 1 != matDst.channels())
			return false;
		int nHeight = matDst.rows;
		int nWidth = matDst.cols;
		int nDstPixNum = nHeight * nWidth;
		int nSrcPixNum = 0;

		int arraySrcNum[256] = { 0 };                // 源图像各灰度统计个数
		int arrayDstNum[256] = { 0 };                // 目标图像个灰度统计个数
		double arraySrcProbability[256] = { 0.0 };   // 源图像各个灰度概率
		double arrayDstProbability[256] = { 0.0 };   // 目标图像各个灰度概率
		// 统计源图像
		for (int j = 0; j < nHeight; j++) {
			for (int i = 0; i < nWidth; i++) {
				arrayDstNum[matDst.at<uchar>(j, i)]++;
			}
		}
		// 统计目标图像
		nHeight = matSrc.rows;
		nWidth = matSrc.cols;
		nSrcPixNum = nHeight * nWidth;
		for (int j = 0; j < nHeight; j++) {
			for (int i = 0; i < nWidth; i++) {
				arraySrcNum[matSrc.at<uchar>(j, i)]++;
			}
		}
		// 计算概率
		for (int i = 0; i < 256; i++) {
			arraySrcProbability[i] = (double)(1.0 * arraySrcNum[i] / nSrcPixNum);
			arrayDstProbability[i] = (double)(1.0 * arrayDstNum[i] / nDstPixNum);
		}
		// 构建直方图均衡映射
		int L = 256;
		int arraySrcMap[256] = { 0 };
		int arrayDstMap[256] = { 0 };
		for (int i = 0; i < L; i++) {
			double dSrcTemp = 0.0;
			double dDstTemp = 0.0;
			for (int j = 0; j <= i; j++) {
				dSrcTemp += arraySrcProbability[j];
				dDstTemp += arrayDstProbability[j];
			}
			arraySrcMap[i] = (int)((L - 1) * dSrcTemp + 0.5);// 减去1，然后四舍五入
			arrayDstMap[i] = (int)((L - 1) * dDstTemp + 0.5);// 减去1，然后四舍五入
		}
		// 构建直方图匹配灰度映射
		int grayMatchMap[256] = { 0 };
		for (int i = 0; i < L; i++) // i表示源图像灰度值
		{
			int nValue = 0;    // 记录映射后的灰度值
			int nValue_1 = 0;  // 记录如果没有找到相应的灰度值时，最接近的灰度值
			int k = 0;
			int nTemp = arraySrcMap[i];
			for (int j = 0; j < L; j++) // j表示目标图像灰度值
			{
				// 因为在离散情况下，之风图均衡化函数已经不是严格单调的了，
				// 所以反函数可能出现一对多的情况，所以这里做个平均。
				if (nTemp == arrayDstMap[j]) {
					nValue += j;
					k++;
				}
				if (nTemp < arrayDstMap[j]) {
					nValue_1 = j;
					break;
				}
			}
			if (k == 0)// 离散情况下，反函数可能有些值找不到相对应的，这里去最接近的一个值
			{
				nValue = nValue_1;
				k = 1;
			}
			grayMatchMap[i] = nValue / k;
		}
		// 构建新图像
		matRet = Mat::zeros(nHeight, nWidth, CV_8UC1);
		for (int j = 0; j < nHeight; j++) {
			for (int i = 0; i < nWidth; i++) {
				matRet.at<uchar>(j, i) = grayMatchMap[matSrc.at<uchar>(j, i)];
			}
		}
		return true;
	}

	void signImage(Mat complex, Mat signIm, Mat &out) {
		Mat planes[] = { Mat::zeros(complex.size(),CV_32F), Mat::zeros(complex.size(),CV_32F) };
		split(complex, planes);

		auto size = min(planes[0].rows*INSERT_SCALE, planes[0].cols*INSERT_SCALE);

		Mat signI = Mat::zeros(size, size, signI.type());
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

