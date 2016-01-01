#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

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

class VideoTransformation
{
public:
    virtual Mat transform(const Mat& frame) const = 0;
};

class TransformationDisplay
{
private:
    string name;

    VideoTransformation& transformation;
public:
    TransformationDisplay(string name, VideoTransformation& t)
        : name(name), transformation(t)
    {
        namedWindow(name, WINDOW_AUTOSIZE);
    }

    void display(const Mat& frame) const
    {
        auto result = transformation.transform(frame);
        imshow(name, result);
    }
};

class NullTransformation: public VideoTransformation
{
public:
    Mat transform(const Mat& frame) const override
    {
        return frame;
    }
public:
};

class GrayscaleTransformation: public VideoTransformation
{
public:
    Mat transform(const Mat& frame) const override
    {
        Mat grayscale;
        cvtColor(frame, grayscale, COLOR_BGRA2GRAY);
        return grayscale;
    }
};

class SegmentedTransformation: public VideoTransformation
{
private:
    vector<Rect> segments;

public:
    Mat transform(const Mat& frame) const override {
        Mat result = frame.clone();
        for (auto i = segments.begin(); i != segments.end(); ++i)
        {
            Mat segment = result(*i);
            transform_segment(segment);
        }
        return result;
    }

protected:
    virtual void transform_segment(Mat& r) const = 0;

public:
    SegmentedTransformation(VideoCapture video, int segment_width, int segment_height)
    {
        double width = video.get(CAP_PROP_FRAME_WIDTH);
        double height = video.get(CAP_PROP_FRAME_HEIGHT);
        if (segment_width == 0)
            segment_width = width;
        if (segment_height == 0)
            segment_height = height;
        Segmentation segmentation(segment_width, segment_height);
        segments = segmentation.get_segments(width, height);
    }
};

class AveragingTransformation: public SegmentedTransformation
{
protected:
    void transform_segment(Mat& segment) const override
    {
        // calculate mean
        Scalar m = mean(segment);
        // set all pixels to the mean
        segment = m;
    }

public:
    AveragingTransformation(const VideoCapture& video, int segment_width, int segment_height)
        : SegmentedTransformation(video, segment_width, segment_height)
    {
    }
};

int process(VideoCapture& capture) {

    double width = capture.get(CAP_PROP_FRAME_WIDTH);
    double height = capture.get(CAP_PROP_FRAME_HEIGHT);

    NullTransformation o;
    TransformationDisplay original("original", o);

    GrayscaleTransformation g;
    TransformationDisplay grayscale("grayscale", g);

    AveragingTransformation h(capture, width, 1);
    TransformationDisplay horizontal("horizontal", h);

    AveragingTransformation v(capture, 1, height);
    TransformationDisplay vertical("vertical", v);

    AveragingTransformation p(capture, 8, 8);
    TransformationDisplay pixelated("pixelated", p);

    Mat frame;

    double frame_count = capture.get(CAP_PROP_FRAME_COUNT);
    int current_frame = 0;
    for (;;) {
        capture >> frame;
        if (frame.empty())
            break;
        cout << current_frame++ << "/" << frame_count << endl;

        original.display(frame);
        grayscale.display(frame);
        horizontal.display(frame);
        vertical.display(frame);
        pixelated.display(frame);

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