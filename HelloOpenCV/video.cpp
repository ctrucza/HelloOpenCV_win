#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

void process_roi(Mat& r)
{
    // calculate mean
    Scalar m = mean(r);
    // set all pixels to the mean
    r = m;
}

vector<Rect> get_rois(const Mat& frame)
{
    int width = frame.cols;
    int height = frame.rows;

    int roi_width = 80;
    int roi_height = 1;

    int roi_per_col = width / roi_width;
    int roi_per_row = height / roi_height;

    vector<Rect> result;
    for (int roi_col = 0; roi_col < roi_per_col; ++roi_col)
    {
        for (int roi_row = 0; roi_row < roi_per_row; ++roi_row)
        {
            Rect roi(roi_col*roi_width, roi_row*roi_height, roi_width, roi_height);
            result.push_back(roi);
        }
    }
    return result;
}

Mat process_frame(Mat& frame)
{
    vector<Rect> rois = get_rois(frame);
    for (auto i = rois.begin(); i != rois.end(); ++i)
    {
        Mat roi_img = frame(*i);
        process_roi(roi_img);
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

        char key = static_cast<char>(waitKey(1)); //delay N millis, usually long enough to display and capture input

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
    if (ac != 2) {
        cerr << "Usage: " << av[0] << " videofile" << endl;
        return 1;
    }
    std::string arg = av[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file or image sequence
    if (!capture.isOpened()) {
        cerr << "Failed to open the video file!\n" << endl;
        return 1;
    }
    process(capture);
}