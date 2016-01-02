#pragma once
#include "SegmentedTransformation.h"

class PredictorSegment
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
	mutable std::map<std::vector<uchar>, cv::Mat> m;
public:
	cv::Rect rect;
	cv::Mat mat;

	PredictorSegment(const cv::Rect& rect)
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
