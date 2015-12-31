//
//  main.cpp
//  HelloOpenCV
//
//  Created by Csaba Trucza on 22/12/15.
//  Copyright © 2015 Csaba Trucza. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

Mat f(Mat& roi)
{
	cvtColor(roi, roi, COLOR_BGRA2GRAY);
	return roi;

}

//void process_roi(Mat& roi)
//{
//	for (int x = 0; x < roi.cols; ++x)
//	{
//		for (int y = 0; y < roi.rows; ++y)
//		{
//			Vec3b color = roi.at<Vec3b>(Point(x, y));
//			//color[0] = 0;
//			//color[1] = 0;
//			color[2] = 255;
//			roi.at<Vec3b>(Point(x, y)) = color;
//		}
//	}
//	Mat g = roi;
//	cvtColor(roi, roi, COLOR_BGRA2GRAY);
//	//roi = g;
//}

void process_roi(Mat& r, int red, int green, int blue)
{
	double sum = 0;
	for (int x = 0; x < r.cols; ++x)
	{
		for (int y = 0; y < r.rows; ++y)
		{
			//auto color = r.at<uchar>(Point(x, y));
			////Vec3b color = r.at<Vec3b>(Point(x, y));
			////color[0] = blue;
			////color[1] = green;
			////color[2] = red;

			//double red_boost = red / 255.0;
			//double green_boost = green / 255.0;
			//double boost = (red_boost + green_boost) / 2.0;

			//r.at<uchar>(Point(x, y)) = boost*color;
			sum += r.at<uchar>(Point(x, y));
		}
	}
	double value = sum / (r.rows*r.cols);
	for (int x = 0; x < r.cols; ++x)
	{
		for (int y = 0; y < r.rows; ++y)
		{
			r.at<uchar>(Point(x, y)) = value;
		}
	}
}

Mat process_frame(Mat& frame)
{
	int width = frame.cols;
	int height = frame.rows;

	int pwidth = 80;
	int pheight = 1;

	int pcols = width / pwidth;
	int prows = height / pheight;

	for (int c = 0; c < pcols; ++c)
	{
		for (int r = 0; r < prows; ++r)
		{
			int red = 255 * (double(r) / double(prows));
			int green = 255 * (double(c) / double(pcols));
			Rect roi(c*pwidth, r*pheight, pwidth, pheight);
			Mat roi_img = frame(roi);
			process_roi(roi_img, red, green, 0);
		}
	}

	return frame;
}

int process(VideoCapture& capture) {
	string original_window_name = "video | q or esc to quit";
	namedWindow(original_window_name, WINDOW_AUTOSIZE);

	string processed_window_name = "processed";
	namedWindow(processed_window_name, WINDOW_AUTOSIZE);

    Mat frame;
	Mat processed_frame;
	double width = capture.get(CAP_PROP_FRAME_WIDTH);
	double height = capture.get(CAP_PROP_FRAME_HEIGHT);

	double frame_count = capture.get(CAP_PROP_FRAME_COUNT);
	int current_frame = 0;
    for (;;) {
        capture >> frame;
        if (frame.empty())
            break;
		cout << current_frame++ << "/" << frame_count << endl;

		imshow(original_window_name, frame);
		Mat grayscale;
		cvtColor(frame, grayscale, COLOR_BGRA2GRAY);

		processed_frame = process_frame(grayscale);
		imshow(processed_window_name, processed_frame);

        char key = (char)waitKey(1); //delay N millis, usually long enough to display and capture input
        
        switch (key) {
            case 'q':
            case 'Q':
            case 27: //escape key
                return 0;
            default:
                break;
        }
    }
    return 0;
}

int main(int ac, char** av) {
//    if (ac != 2) {
//        return 1;
//    }
    std::string arg = av[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file or image sequence
    if (!capture.isOpened()) {
        cerr << "Failed to open the video file!\n" << endl;
        return 1;
    }
    process(capture);
}