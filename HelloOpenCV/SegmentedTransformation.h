#pragma once
#include <opencv2/core.hpp>
#include "VideoTransformation.h"
#include "Segmentation.h"

class Segment
{
private:
    inline std::vector<uchar> to_vector(const cv::Mat& mat)
    {
        std::vector<uchar> result;
        if (mat.isContinuous()) {
            result.assign(mat.datastart, mat.dataend);
        }
        else {
            for (int i = 0; i < mat.rows; ++i) {
                result.insert(result.end(), mat.ptr<uchar>(i), mat.ptr<uchar>(i) + mat.cols);
            }
        }
        return result;
    }

    mutable std::vector<uchar> last;
    static std::map<std::vector<uchar>, cv::Mat> m;
public:
    cv::Rect rect;
    cv::Mat mat;
public:
    Segment(const cv::Rect& rect)
        :rect(rect)
    {}

    void predict()
    {
        auto copy = to_vector(mat);

        m[last] = mat;
        last = copy;

        auto result = m.find(copy);
        if (result == m.end())
            mat = 0;
        else
            mat = result->second;

    }
};

std::map<std::vector<uchar>, cv::Mat> Segment::m;

class SegmentedTransformation : public VideoTransformation
{
private:
    mutable std::vector<Segment*> segments;

public:
    cv::Mat transform(const cv::Mat& frame) const override {
        cv::Mat result = frame.clone();
        for (std::vector<Segment*>::iterator i = segments.begin(); i != segments.end(); ++i)
        {
            cv::Mat roi = result((*i)->rect);
            (*i)->mat = roi;
            transform_segment(**i);
        }
        return result;
    }

protected:
    virtual void transform_segment(Segment& segment) const = 0;

public:
    SegmentedTransformation(int width, int height, int segment_width, int segment_height)
    {
        Segmentation segmentation(segment_width, segment_height);
        auto rects = segmentation.get_segments(width, height);
        for (auto i = rects.begin(); i != rects.end(); ++i)
            segments.push_back(new Segment(*i));
    }
};

class AveragingTransformation : public SegmentedTransformation
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

