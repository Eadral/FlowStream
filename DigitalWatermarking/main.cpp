#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "FT.hpp"

using namespace std;
using namespace cv;


int main() {

	Mat origin = imread("origin.png");
	imshow("origin", origin);
	Mat signI = imread("sign.png", IMREAD_GRAYSCALE);
	//imshow("sign", signI);

	vector<Mat> originChannels;
	split(origin, originChannels);

	Mat blue = originChannels.at(0);

	imshow("blue", blue);
	Mat complexI;
	img_dft(blue, complexI);

	Mat magI = show_mag(complexI);
	//imshow("dft", magI);

	Mat iDft = img_idft(complexI);
	//imshow("idft", iDft);


	Mat mod;
	complexI.copyTo(mod);

	shift(mod);

	Mat magM = show_mag(mod);
	//imshow("shifted", magM);

	sign(mod, signI, mod);


	Mat signd = show_mag(mod);
	//imshow("signed", signd);

	shift(mod);

	Mat signd_idft = img_idft(mod);
	//imshow("signd_idft", signd_idft);

	signd_idft.convertTo(signd_idft, CV_32F, 1.0, 0.14);

	Mat recoverChannel;
	signd_idft.copyTo(recoverChannel);
	recoverChannel = recoverChannel.mul(255);
	recoverChannel.convertTo(recoverChannel, CV_8U);

	auto x = signd_idft.at<float>(0, 0);
	auto y = recoverChannel.at<uchar>(0, 0);

	//Mat originRoi = originChannels[0](Rect(0, 0, signd_idft.cols, signd_idft.rows)); 
	//addWeighted(originRoi, 0., signd_idft, 1., 0., originRoi);
	originChannels[0] = recoverChannel;
	imshow("after_blue", signd_idft);


	Mat after;
	merge(originChannels, after);
	imshow("after", after);

	Mat next;
	img_dft(signd_idft, next);

	shift(next);
	Mat finaled = show_mag(next);
	imshow("final", finaled);

	Mat qrRoi = finaled(Rect(0, 0, finaled.cols*insert_scale, finaled.rows*insert_scale));
	Mat qr;
	qrRoi.copyTo(qr);
	imshow("QR", qr);
	qr.convertTo(qr, -1, 10, 0);
	//equalizeHist(qr, qr);
	imshow("EnhancedQR", qr);

	waitKey(0);
	return 0;
}

