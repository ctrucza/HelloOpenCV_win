#include <opencv2/opencv.hpp>

#include <iostream>
#include "VideoTransformation.h"
#include "TransformationDisplay.h"
#include "SegmentedTransformation.h"
#include "PredictorTransformation.h"
#include <strstream>

using namespace cv;
using namespace std;


int process(VideoCapture& capture) {

    int width = int(capture.get(CAP_PROP_FRAME_WIDTH));
    int height = int(capture.get(CAP_PROP_FRAME_HEIGHT));
    vector<TransformationDisplay*> displays;

    GrayscaleTransformation g;

    PredictorTransformation predictor(width, height, 8, 8);

    CombinigTransformation combiner;
    combiner.add(&g);
    combiner.add(&predictor);

    Size size(width, 2 * height);
    VideoFileWriter writer("foo.avi", -1, 30, size, false);

    ChainedTransformation pipeline;
    pipeline.add(&g);
    pipeline.add(&predictor);
    pipeline.add(&combiner);
    pipeline.add(&writer);

    TransformationDisplay view("view", pipeline);
    displays.push_back(&view);

    //TransformationDisplay grayscale("grayscale", g);
    //displays.push_back(&grayscale);

    //TransformationDisplay prediction("prediction", pipeline);
    //displays.push_back(&prediction);
    
    Mat frame;

    double frame_count = capture.get(CAP_PROP_FRAME_COUNT);
    int current_frame = 0;


    for (;;) {
        capture >> frame;
        if (frame.empty())
            break;
        cout << current_frame++ << "/" << frame_count << endl;

        for (auto display = displays.begin(); display != displays.end(); ++display)
        {
            (*display)->display(frame);
        }

        char key = static_cast<char>(waitKey(1));
        
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
//
//void print_map(const SimilarPatterns& patterns)
//{
//    static int counter = 0;
//
//    int image_count = patterns.size();
//    int image_width = patterns[0].cols;
//    int image_height = patterns[0].rows;
//
//    int adjusted_height = image_height + 1;
//
//    int total_height = image_count * adjusted_height;
//
//    cv::Mat result(total_height, image_width, patterns[0].type());
//    int y = 0;
//    for (auto i = patterns.begin(); i != patterns.end(); ++i)
//    {
//        cv::Rect r(0, y*adjusted_height, image_width, image_height);
//        cv::Mat roi = result(r);
//        i->copyTo(roi);
//        y++;
//    }
//    ostrstream filename;
//    filename << "patterns_" << counter << ".png" << ends;
//    string f = filename.str();
//    imwrite(f, result);
//    counter++;
//}

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
