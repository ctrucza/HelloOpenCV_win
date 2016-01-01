#pragma once

class Segmentation
{
private:
    int segment_width;
    int segment_height;
public:
    Segmentation(int width, int height)
        : segment_width(width), segment_height(height)
    {

    }

    std::vector<cv::Rect> get_segments(int width, int height) const
    {
        int segments_per_col = width / segment_width;
        int segments_per_row = height / segment_height;

        std::vector<cv::Rect> result;
        for (int segment_col = 0; segment_col < segments_per_col; ++segment_col)
        {
            for (int segment_row = 0; segment_row < segments_per_row; ++segment_row)
            {
                cv::Rect roi(segment_col*segment_width, segment_row*segment_height, segment_width, segment_height);
                result.push_back(roi);
            }
        }
        return result;
    }
};
