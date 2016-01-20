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
    int differences = cv::countNonZero(diff);
    return differences == 0;
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

public:
    std::vector<cv::Mat> patterns;

    typedef std::vector<cv::Mat> SimilarPatterns;
    std::vector<SimilarPatterns> similarityMap;
    double similarity_threshold = 0.8;

private:
    bool matches(const cv::Mat& other)
    {
        cv::Mat diff;
        cv::absdiff(mat, other, diff);
        int d = cv::countNonZero(diff);
        int t = mat.rows*mat.cols*similarity_threshold;
        return d < t;
    }

    bool matches(const SimilarPatterns& patterns)
    {
        for (auto i = patterns.begin(); i != patterns.end(); ++i)
            if (matches(*i))
                return true;
        return false;        
    }

    void add_new_pattern()
    {
        patterns.push_back(mat.clone());
        for (int i = 0; i < similarityMap.size(); ++i)
        {
            if (matches(similarityMap[i]))
            {
                similarityMap[i].push_back(mat.clone());
                return;
            }
        }
        SimilarPatterns new_pattern;
        new_pattern.push_back(mat.clone());
        similarityMap.push_back(new_pattern);
    }
public:
	PredictorSegment(const cv::Rect& rect)
		:Segment(rect)
	{}

	void predict()
	{
        if (std::find_if(patterns.begin(), patterns.end(), mat_compare(mat)) == patterns.end())
        {
            add_new_pattern();
        }

		auto patch = to_patch(mat);

		patches[last] = mat;
		last = patch;

		auto result = patches.find(patch);
		if (result == patches.end())
			mat = 255;
		else
			mat = result->second;
	}
};

//std::vector<cv::Mat> PredictorSegment::patterns;
//std::vector<SimilarPatterns> PredictorSegment::similarityMap;

class PredictorTransformation: public SegmentedTransformation<PredictorSegment>
{
protected:
    void transform_segment(PredictorSegment& segment) const override 
    {
        segment.predict();
    }

public:
    PredictorTransformation(int width, int height, int segment_width, int segment_height)
        : SegmentedTransformation(width, height, segment_width, segment_height)
    {
    }
};
