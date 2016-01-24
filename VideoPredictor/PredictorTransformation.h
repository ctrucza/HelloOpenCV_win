#pragma once
#include "SegmentedTransformation.h"

// As map<Mat, something> is tricky (operator< for Mat is hard to define),
// we convert Mat to Patch when we want to compare Mats
typedef std::vector<uchar> Patch;
inline Patch to_patch(const cv::Mat& mat)
{
	Patch result;
	for (int i = 0; i < mat.rows; ++i) {
		result.insert(result.end(), mat.ptr<uchar>(i), mat.ptr<uchar>(i) + mat.cols);
	}
	return result;
}

inline bool compare_matrices(const cv::Mat& lhs, const cv::Mat& rhs)
{
    cv::Mat diff;
    cv::absdiff(lhs, rhs, diff);
    
    int min_diff = 255;
    int max_diff = 0;
    int total_diff = 0;

    for (int row = 0; row < diff.rows; ++row)
        for (int col = 0; col < diff.cols; ++col)
        {
            int d = diff.at<uchar>(row,col);
            total_diff += d;
            if (d < min_diff)
                min_diff = d;
            if (d > max_diff)
                max_diff = d;
        }

    int avg_diff = total_diff / (diff.rows*diff.cols);
    return (avg_diff < 20) && (max_diff < 40);
}

// comparator used in finding Mat's in vector<Mat>
struct mat_compare : public std::unary_function<cv::Mat, bool>
{
    explicit mat_compare(const cv::Mat &baseline) : baseline(baseline) {}
    bool operator() (const cv::Mat &arg) const
    {
        return compare_matrices(arg, baseline);
    }
    cv::Mat baseline;
};

class PredictorSegment: public Segment
{
private:
	Patch last;
	std::map<Patch, cv::Mat> patches;
    std::vector<cv::Mat> patterns;

public:
	PredictorSegment(const cv::Rect& rect)
		:Segment(rect)
	{}

	virtual void transform() override
	{
        cv::Mat pattern;
        auto match = std::find_if(patterns.begin(), patterns.end(), mat_compare(mat));
        if ( match == patterns.end())
        {
            patterns.push_back(mat.clone());
            pattern = mat;
        }
        else
        {
            pattern = *match;
        }

		auto patch = to_patch(pattern);
        if (patches.find(last) == patches.end())
        {
            patches[last] = pattern;
        }
		last = patch;

		auto result = patches.find(patch);
		if (result == patches.end())
			mat = 255;
		else
			mat = result->second;
	}


    void dump()
	{
        std::cout << "segment ";
        std::cout << "patterns: " << patterns.size() << " ";
        std::cout << "patches: " << patches.size() << std::endl;
	}
};

//std::map<Patch, cv::Mat> PredictorSegment::patches;
//std::vector<cv::Mat> PredictorSegment::patterns;

class PredictorTransformation: public SegmentedTransformation<PredictorSegment>
{
public:
    PredictorTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }


    void dump()
    {
        auto dump_segment = [](PredictorSegment& segment) {segment.dump(); };
        visit_segments(dump_segment);
    }
};
