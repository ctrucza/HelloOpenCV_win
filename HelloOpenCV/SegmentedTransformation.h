#pragma once
#include <opencv2/core.hpp>
#include "VideoTransformation.h"
#include "Segmentation.h"

class SegmentedTransformation : public VideoTransformation
{
private:
    std::vector<cv::Rect> segments;

public:
    cv::Mat transform(const cv::Mat& frame) const override {
        cv::Mat result = frame.clone();
        for (auto i = segments.begin(); i != segments.end(); ++i)
        {
            cv::Mat segment = result(*i);
            transform_segment(segment);
        }
        return result;
    }

protected:
    virtual void transform_segment(cv::Mat& r) const = 0;

public:
    SegmentedTransformation(int width, int height, int segment_width, int segment_height)
    {
        Segmentation segmentation(segment_width, segment_height);
        segments = segmentation.get_segments(width, height);
    }
};

class AveragingTransformation : public SegmentedTransformation
{
protected:
    void transform_segment(cv::Mat& segment) const override
    {
        // calculate mean
        cv::Scalar m = cv::mean(segment);
        // set all pixels to the mean
        segment = m;
    }

public:
    AveragingTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};

