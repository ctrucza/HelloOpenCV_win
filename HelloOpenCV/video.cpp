#include <opencv2/opencv.hpp>

#include <iostream>
#include "VideoTransformation.h"
#include "TransformationDisplay.h"
#include "SegmentedTransformation.h"
#include "PredictorTransformation.h"

using namespace cv;
using namespace std;


int process(VideoCapture& capture) {

    int width = int(capture.get(CAP_PROP_FRAME_WIDTH));
    int height = int(capture.get(CAP_PROP_FRAME_HEIGHT));

    NullTransformation o;
    TransformationDisplay original("original", o);

    GrayscaleTransformation g;
    TransformationDisplay grayscale("grayscale", g);

    AveragingTransformation h(width, height, width, 1);
    TransformationDisplay horizontal("horizontal", h);

    AveragingTransformation v(width, height, 1, height);
    TransformationDisplay vertical("vertical", v);

    AveragingTransformation p(width, height, 8, 8);
    TransformationDisplay pixelated("pixelated", p);

    ChainedTransformation c;
    c.add(&g);
    c.add(&p);
    TransformationDisplay chained("chained", c);

    ChainedTransformation null_transformation;
    TransformationDisplay null_display("null", null_transformation);

    PredictorTransformation predictor(width, height, 16, 16);
    TransformationDisplay prediction("color prediction", predictor);

    ChainedTransformation grayscale_predictor;
    grayscale_predictor.add(&g);
    //loose.add(&p);
    grayscale_predictor.add(&predictor);
    TransformationDisplay grayscale_prediction("grayscale prediction", grayscale_predictor);

    Mat frame;

    double frame_count = capture.get(CAP_PROP_FRAME_COUNT);
    int current_frame = 0;
    for (;;) {
        capture >> frame;
        if (frame.empty())
            break;
        cout << current_frame++ << "/" << frame_count << endl;

        original.display(frame);
        //grayscale.display(frame);
        //horizontal.display(frame);
        //vertical.display(frame);
        //pixelated.display(frame);
        //chained.display(frame);
        //null_display.display(frame);
        prediction.display(frame);
        grayscale_prediction.display(frame);

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