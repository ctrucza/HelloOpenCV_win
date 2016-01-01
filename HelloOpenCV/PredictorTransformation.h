#pragma once
#include "SegmentedTransformation.h"

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

class Predictor
{
private:
    mutable std::vector<uchar> last;
    mutable std::map<std::vector<uchar>, cv::Mat> m;
public:
    void predict(cv::Mat input) const
    {
        auto copy = to_vector(input);

        m[last] = input;
        last = copy;

        auto result = m.find(copy);
        if (result == m.end())
            input = 0;
        else
            input = result->second;
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
