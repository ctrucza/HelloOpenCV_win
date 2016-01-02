#pragma once
#include <opencv2/core.hpp>
#include "VideoTransformation.h"
#include "Segmentation.h"

class Segment
{
protected:
	cv::Rect rect;
	cv::Mat mat;

public:
    Segment(const cv::Rect& rect)
        :rect(rect)
    {}

	void prepare(cv::Mat& frame)
    {
		mat = frame(rect);
    }
};

template <class SegmentType>
class SegmentedTransformation : public VideoTransformation
{
private:
    mutable std::vector<SegmentType> segments;

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

	cv::Mat transform(const cv::Mat& frame) const override {
		cv::Mat result = frame.clone();
		for (auto i = segments.begin(); i != segments.end(); ++i)
		{
			i->prepare(result);
			transform_segment(*i);
		}
		return result;
	}
};

class AveragingSegment: public Segment
{
public:
	AveragingSegment(const cv::Rect& rect)
		: Segment(rect)
	{
	}

	void calculate_average()
	{
		// calculate mean
		cv::Scalar m = cv::mean(mat);
		// set all pixels to the mean
		mat = m;
	}
};

class AveragingTransformation : public SegmentedTransformation<AveragingSegment>
{
protected:
    void transform_segment(AveragingSegment& segment) const override
    {
		segment.calculate_average();
    }

public:
    AveragingTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};

