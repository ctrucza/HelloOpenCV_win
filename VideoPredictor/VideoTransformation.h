#pragma once
#include <opencv2/core/mat.hpp>

class VideoTransformation
{
protected:
    virtual cv::Mat do_transform(const cv::Mat& frame) const = 0;
public:
    mutable cv::Mat last_frame;
    cv::Mat transform(const cv::Mat& frame) const
    {
        auto result = do_transform(frame);
        last_frame = result.clone();
        return result;
    };
};

class VideoFileWriter: public VideoTransformation
{
private:
    mutable cv::VideoWriter output;

protected:
    cv::Mat do_transform(const cv::Mat& frame) const override
    {
        output << frame;
        return frame;
    }

public:
    VideoFileWriter(std::string fileName, int fourcc, double fps, cv::Size& frameSize, bool isColor)
    {
        output.open(fileName, fourcc, fps, frameSize, isColor);
        if (!output.isOpened())
            throw "cannot open output";
    }
};

class CombinigTransformation: public VideoTransformation
{
private:
    std::vector<VideoTransformation*> sources;
protected:
    cv::Mat do_transform(const cv::Mat& frame) const override {

        int height = frame.rows;
        int width = frame.cols;
        cv::Mat combined(sources.size()*height, width, (sources[0])->last_frame.type());
        int stream_no = 0;
        for (auto source : sources)
        {
            source->last_frame;

            cv::Rect original_rect(0, stream_no*height, width, height);
            cv::Mat original_roi = combined(original_rect);
            source->last_frame.copyTo(original_roi);
            stream_no++;
        }
        return combined;
    }
public:
    void add(VideoTransformation* transformation)
    {
        sources.push_back(transformation);
    }
};

class ChainedTransformation : public VideoTransformation
{
private:
    std::vector<VideoTransformation*> transformations;
protected:
    cv::Mat do_transform(const cv::Mat& frame) const override {
        auto result = frame;
        for (auto i = transformations.begin(); i != transformations.end(); ++i)
        {
            auto output = (*i)->transform(result);
            result = output;
        }
        return result;
    }
public:
    void add(VideoTransformation* t)
    {
        transformations.push_back(t);
    }
};

class NullTransformation : public VideoTransformation
{
protected:
    cv::Mat do_transform(const cv::Mat& frame) const override
    {
        return frame;
    }
};

class GrayscaleTransformation : public VideoTransformation
{
protected:
    cv::Mat do_transform(const cv::Mat& frame) const override
    {
        cv::Mat grayscale;
        cvtColor(frame, grayscale, cv::COLOR_BGRA2GRAY);
        return grayscale;
    }
};

