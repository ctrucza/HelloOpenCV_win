#pragma once
#include "SegmentedTransformation.h"

typedef std::vector<uchar> Patch;

inline Patch to_patch(const cv::Mat& mat)
{
	Patch result;
	for (int i = 0; i < mat.rows; ++i) {
		result.insert(result.end(), mat.ptr<uchar>(i), mat.ptr<uchar>(i) + mat.cols);
	}
	return result;
}

class PredictorSegment: public Segment
{
private:
	Patch last;
	std::map<Patch, cv::Mat> m;
public:
	PredictorSegment(const cv::Rect& rect)
		:Segment(rect)
	{}

	void predict()
	{
		auto copy = to_patch(mat);

		m[last] = mat;
		last = copy;

		auto result = m.find(copy);
		if (result == m.end())
			mat = 0;
		else
			mat = result->second;
	}
};

//std::map<std::vector<uchar>, cv::Mat> PredictorSegment::m;

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
