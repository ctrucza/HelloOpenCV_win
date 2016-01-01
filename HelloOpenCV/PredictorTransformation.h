#pragma once
#include "SegmentedTransformation.h"

class PredictorTransformation: public SegmentedTransformation
{
protected:
    void transform_segment(Segment& segment) const override 
    {
        segment.predict();
    }

public:
    PredictorTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};
