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

    //NullTransformation o;
    //TransformationDisplay original("original", o);
    //displays.push_back(&original);

    GrayscaleTransformation g;
    TransformationDisplay grayscale("grayscale", g);
    displays.push_back(&grayscale);

    PredictorTransformation predictor(width, height, 8, 8);
    ChainedTransformation grayscale_predictor;
    grayscale_predictor.add(&g);
    grayscale_predictor.add(&predictor);
    TransformationDisplay grayscale_prediction("grayscale prediction", grayscale_predictor);
    displays.push_back(&grayscale_prediction);

    //AveragingTransformation h(width, height, width, 1);
    //TransformationDisplay horizontal("horizontal", h);
    //displays.push_back(&horizontal);

    //AveragingTransformation v(width, height, 1, height);
    //TransformationDisplay vertical("vertical", v);
    //displays.push_back(&vertical);

    //AveragingTransformation p(width, height, 8, 8);
    //TransformationDisplay pixelated("pixelated", p);
    //displays.push_back(&pixelated);

    //ChainedTransformation c;
    //c.add(&g);
    //c.add(&p);
    //TransformationDisplay chained("chained", c);
    //displays.push_back(&chained);


    Mat frame;

    double frame_count = capture.get(CAP_PROP_FRAME_COUNT);
    int current_frame = 0;
    for (;;) {
        capture >> frame;
        if (frame.empty())
            break;
        cout << current_frame++ << "/" << frame_count << endl;

        for (auto display = displays.begin(); display != displays.end(); ++display)
            (*display)->display(frame);

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
