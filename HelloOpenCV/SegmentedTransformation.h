#pragma once
#include <opencv2/core.hpp>
#include "VideoTransformation.h"
#include "Segmentation.h"

class Segment
{
public:
    cv::Rect rect;
    cv::Mat mat;

    Segment(const cv::Rect& rect)
        :rect(rect)
    {}
};

template <class SegmentType=Segment>
class SegmentedTransformation : public VideoTransformation
{
private:
    mutable std::vector<SegmentType> segments;

public:
    cv::Mat transform(const cv::Mat& frame) const override {
        cv::Mat result = frame.clone();
        for (typename std::vector<SegmentType>::iterator i = segments.begin(); i != segments.end(); ++i)
        {
            cv::Mat roi = result(i->rect);
            i->mat = roi;
            transform_segment(*i);
        }
        return result;
    }

protected:
    virtual void transform_segment(SegmentType& segment) const = 0;

public:
    SegmentedTransformation(int width, int height, int segment_width, int segment_height)
    {
        Segmentation segmentation(segment_width, segment_height);
        auto rects = segmentation.get_segments(width, height);
        for (auto i = rects.begin(); i != rects.end(); ++i)
        {
			SegmentType segment(*i);
			segments.push_back(segment);
        }
    }
};

class AveragingTransformation : public SegmentedTransformation<>
{
protected:
    void transform_segment(Segment& segment) const override
    {
        // calculate mean
        cv::Scalar m = cv::mean(segment.mat);
        // set all pixels to the mean
        segment.mat = m;
    }

public:
    AveragingTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};

