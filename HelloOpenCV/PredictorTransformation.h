#pragma once
#include "SegmentedTransformation.h"

class Predictor
{
private:
    cv::Mat last;
public:
    void predict(cv::Mat input) const
    {
        input = 0;
    }
};

class PredictorTransformation: public SegmentedTransformation
{
private:
    Predictor p;
protected:
    void transform_segment(cv::Mat& segment) const override 
    {
        p.predict(segment);
    }

public:
    PredictorTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};
