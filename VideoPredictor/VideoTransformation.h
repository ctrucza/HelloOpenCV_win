#pragma once
#include <opencv2/core/mat.hpp>

class VideoTransformation
{
public:
    virtual cv::Mat transform(const cv::Mat& frame) const = 0;
};

class ChainedTransformation : public VideoTransformation
{
private:
    std::vector<VideoTransformation*> transformations;
public:
    cv::Mat transform(const cv::Mat& frame) const override {
        auto result = frame;
        for (auto i = transformations.begin(); i != transformations.end(); ++i)
        {
            auto output = (*i)->transform(result);
            result = output;
        }
        return result;
    }

    void add(VideoTransformation* t)
    {
        transformations.push_back(t);
    }
};

class NullTransformation : public VideoTransformation
{
public:
    cv::Mat transform(const cv::Mat& frame) const override
    {
        return frame;
    }
};

class GrayscaleTransformation : public VideoTransformation
{
public:
    cv::Mat transform(const cv::Mat& frame) const override
    {
        cv::Mat grayscale;
        cvtColor(frame, grayscale, cv::COLOR_BGRA2GRAY);
        return grayscale;
    }
};

