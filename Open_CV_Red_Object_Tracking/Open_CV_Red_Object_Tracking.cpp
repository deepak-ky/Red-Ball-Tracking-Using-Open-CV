#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
int main()
{
	VideoCapture cap(0);
	//VideoCapture cap("red_ball_tracking.mp4");
	if (cap.isOpened() == false)
	{
		cout << "Could Not Open Video File";
		return -1;
	}
	string w = "original";
	namedWindow(w, WINDOW_NORMAL);
	string w1 = "Control";
	namedWindow(w1, WINDOW_NORMAL);

	int low_hue = 0;
	int high_hue = 179;

	int low_satur = 0;
	int high_satur = 255;

	int low_value = 0;
	int high_value = 255;

	createTrackbar("MinH", w1, &low_hue, 179);
	createTrackbar("MaxH", w1, &high_hue, 179);

	createTrackbar("MinS", w1, &low_satur, 255);
	createTrackbar("MaxS", w1, &high_satur, 255);

	createTrackbar("MinV", w1, &low_value, 255);
	createTrackbar("MaxV", w1, &high_value, 255);

	int last_x_position = -1;
	int last_y_position = -1;

	Mat imgTmp;
	cap.read(imgTmp);

	Mat imgLines(imgTmp.size(), CV_8UC3,Scalar(0,0,0));

	while (true)
	{
		Mat frame;
		bool b = cap.read(frame);
		if (b == false)
		{
			cout << "Could Not Play Video";
			break;
		}

		Mat imgHSV;

		cvtColor(frame, imgHSV, COLOR_BGR2HSV);

		Mat imgthresold;

		inRange(imgHSV, Scalar(low_hue, low_satur, low_value), Scalar(high_hue, high_satur, high_satur), imgthresold);

		//Opening on the image
		erode(imgthresold, imgthresold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgthresold, imgthresold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//Closing on the image
		dilate(imgthresold, imgthresold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgthresold, imgthresold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		Moments oMoments = moments(imgthresold);
		double m01 = oMoments.m01;
		double m02 = oMoments.m10;
		double m03 = oMoments.m00;
       
		if (m03 > 0)
		{
			int posX = m02 / m03;
			int posY = m01 / m03;

			if (last_x_position >= 0 && last_y_position >= 0 && posX >= 0 && posY >= 0)
			{
				//Draw a line from the previous postion to the current position
				line(imgLines, Point(posX, posY), Point(last_x_position, last_y_position), Scalar(0, 255,0), 2);

			}
			last_x_position = posX;
			last_y_position = posY;
	   }
		imshow("Thresold", imgthresold);

		frame = frame + imgLines;
		imshow(w, frame);

		if (waitKey(30) == 27)
		{
			break;
		}

	}

	return 0;
}