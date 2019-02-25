#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "FT.hpp"

using namespace std;
using namespace cv;



int main() {

	Mat origin = imread("origin.png");
	//imshow("origin", origin);
	Mat signI = imread("sign.png", IMREAD_GRAYSCALE);
	imshow("sign", signI);

	vector<Mat> originChannels;
	split(origin, originChannels);

	Mat blue = originChannels.at(0);

	imshow("blue", blue);
	Mat complexI;
	img_dft(blue, complexI);

	Mat magI = show_mag(complexI);
	imshow("dft", magI);

	Mat iDft = img_idft(complexI);
	imshow("idft", iDft);


	Mat mod;
	complexI.copyTo(mod);

	shift(mod);

	Mat magM = show_mag(mod);
	imshow("shifted", magM);
	
	sign(mod, signI, mod);


	Mat signd = show_mag(mod);
	imshow("signed", signd);

	shift(mod);

	Mat signd_idft = img_idft(mod);
	imshow("signd_idft", signd_idft);

	Mat next;
	img_dft(signd_idft, next);

	shift(next);
	Mat finaled = show_mag(next);
	imshow("final", finaled);

	waitKey(0);
	return 0;
}
