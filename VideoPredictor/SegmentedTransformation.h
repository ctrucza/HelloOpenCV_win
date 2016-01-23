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

    virtual void transform()
    {
        
    }
};


template <class SegmentType>
class SegmentedTransformation : public VideoTransformation
{
private:
    mutable std::vector<SegmentType> segments;

    class ParallelTransform : public cv::ParallelLoopBody
    {
    private:
        std::vector<SegmentType>& segments;
        cv::Mat& result;
    public:
        ParallelTransform(std::vector<SegmentType>& segments, cv::Mat& result)
            : segments(segments), result(result)
        {

        }

        virtual void operator()(const cv::Range& range) const override {
            for (int i = range.start; i != range.end; ++i)
            {
                segments[i].prepare(result);
                segments[i].transform();
            }
        }
    };

protected:
    cv::Mat do_transform(const cv::Mat& frame) const override {
        cv::Mat result = frame.clone();

        auto t = ParallelTransform(segments, result);
        cv::parallel_for_(cv::Range(0, segments.size()), t);
        return result;
    }

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

class AveragingSegment: public Segment
{
public:
	AveragingSegment(const cv::Rect& rect)
		: Segment(rect)
	{
	}

    void transform() override
	{
		// calculate mean
		cv::Scalar m = cv::mean(mat);
		// set all pixels to the mean
		mat = m;
	}
};

class AveragingTransformation : public SegmentedTransformation<AveragingSegment>
{
public:
    AveragingTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};

