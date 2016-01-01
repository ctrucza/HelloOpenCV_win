#pragma once
#include <opencv2/highgui.hpp>
#include "VideoTransformation.h"

class TransformationDisplay
{
private:
    std::string name;

    VideoTransformation& transformation;
public:
    TransformationDisplay(std::string name, VideoTransformation& t)
        : name(name), transformation(t)
    {
        namedWindow(name, cv::WINDOW_AUTOSIZE);
    }

    void display(const cv::Mat& frame) const
    {
        auto result = transformation.transform(frame);
        cv::imshow(name, result);
    }
};

