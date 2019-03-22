#pragma once

#include <opencv2/opencv.hpp>

#include "utility.hpp"
#include "parameters.hpp"

using namespace std;
using namespace cv;
using namespace Meow;

class Test {
public:

	Test (string filename_a, string filename_b)
		: filename_a(filename_a), filename_b(filename_b) {
	}

	void computeValue() {
		const Mat imageA = imread(filename_a);
		const Mat imageB = imread(filename_b);
		cout << "PSNR: " << getPSNR(imageA, imageB) << endl;
		cout << "SSIM: " << getMSSIM(imageA, imageB)[0] << endl;
	}

	void high_pass_fillter() {
		Mat source = imread(filename_a);

		Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
		
		Mat dst;
		filter2D(source, dst, -1, kernel);

		imwrite(filename_b, dst);
	}

	void noise() {
		Mat source = imread(filename_a);
		Mat noise(source.size(), source.type());
		RNG rng(time(NULL));
		rng.fill(noise, RNG::NORMAL, 0, 20);
		Mat dst;
		add(source, noise, dst);
		imwrite(filename_b, dst);
	}

	void cutting() {
		Mat source = imread(filename_a);
		Mat roi = source(cv::Rect(source.cols * 0.25, source.rows * 0.25,
			source.cols * 0.5, source.rows * 0.5));
		Mat zero = Mat(roi.size(), roi.type());
		addWeighted(roi, 0, zero, 1, 0, roi);
		imwrite(filename_b, source);

	}

private:

	//输入格式是Mat类型，I1，I2代表是输入的两幅图像
	double getPSNR(const Mat& I1, const Mat& I2)
	{
		Mat s1;
		absdiff(I1, I2, s1);       // |I1 - I2|AbsDiff函数是 OpenCV 中计算两个数组差的绝对值的函数
		s1.convertTo(s1, CV_32F);  // 这里我们使用的CV_32F来计算，因为8位无符号char是不能进行平方计算
		s1 = s1.mul(s1);           // |I1 - I2|^2

		Scalar s = sum(s1);         //对每一个通道进行加和

		double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

		if (sse <= 1e-10) // 对于非常小的值我们将约等于0
			return 0;
		else
		{
			double  mse = sse / (double)(I1.channels() * I1.total());//计算MSE
			double psnr = 10.0*log10((255 * 255) / mse);
			return psnr;//返回PSNR
		}
	}

	Scalar getMSSIM(Mat  inputimage1, Mat inputimage2)
	{
		Mat i1 = inputimage1;
		Mat i2 = inputimage2;
		const double C1 = 6.5025, C2 = 58.5225;
		int d = CV_32F;
		Mat I1, I2;
		i1.convertTo(I1, d);
		i2.convertTo(I2, d);
		Mat I2_2 = I2.mul(I2);
		Mat I1_2 = I1.mul(I1);
		Mat I1_I2 = I1.mul(I2);
		Mat mu1, mu2;
		GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
		GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
		Mat mu1_2 = mu1.mul(mu1);
		Mat mu2_2 = mu2.mul(mu2);
		Mat mu1_mu2 = mu1.mul(mu2);
		Mat sigma1_2, sigma2_2, sigma12;
		GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
		sigma1_2 -= mu1_2;
		GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
		sigma2_2 -= mu2_2;
		GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
		sigma12 -= mu1_mu2;
		Mat t1, t2, t3;
		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);
		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);
		Mat ssim_map;
		divide(t3, t1, ssim_map);
		Scalar mssim = mean(ssim_map);
		return mssim;
	}

private:
	string filename_a;
	string filename_b;

};