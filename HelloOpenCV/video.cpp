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

class Segmentation
{
private:
    int segment_width;
    int segment_height;
public:
    Segmentation(int width, int height)
        : segment_width(width), segment_height(height)
    {
        
    }

    vector<Rect> get_segments(int width, int height) const
    {
        int segments_per_col = width / segment_width;
        int segments_per_row = height / segment_height;

        vector<Rect> result;
        for (int segment_col = 0; segment_col < segments_per_col; ++segment_col)
        {
            for (int segment_row = 0; segment_row < segments_per_row; ++segment_row)
            {
                Rect roi(segment_col*segment_width, segment_row*segment_height, segment_width, segment_height);
                result.push_back(roi);
            }
        }
        return result;
    }
};

Mat process_frame(Mat& frame, const vector<Rect> segments)
{
    for (auto i = segments.begin(); i != segments.end(); ++i)
    {
        Mat roi_img = frame(*i);
        process_roi(roi_img);
    }
    return frame;
}

int process(VideoCapture& capture) {

    double width = capture.get(CAP_PROP_FRAME_WIDTH);
    double height = capture.get(CAP_PROP_FRAME_HEIGHT);

    Segmentation s(80, 1);
    vector<Rect> segments = s.get_segments(width, height);

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

        processed_frame = process_frame(grayscale, segments);
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