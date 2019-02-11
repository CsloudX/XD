#ifndef _X_COLOR_DETECTOR_HPP
#define _X_COLOR_DETECTOR_HPP

#include "XMat.hpp"

class XColorDetector
{
public:
	XColorDetector(const cv::Vec3b& targetColor,int minDist=100)
		:m_targetColor(targetColor),m_minDist(minDist){}
	cv::Mat process(const cv::Mat& image)
	{
		cv::Mat result;
		result.create(image.size(), CV_8U);
		cv::Mat_<cv::Vec3b>::const_iterator itr = image.begin<cv::Vec3b>();
		cv::Mat_<cv::Vec3b>::const_iterator itrEnd = image.end<cv::Vec3b>();
		cv::Mat_<uchar>::iterator itrOut = result.begin<uchar>();
		for (; itr != itrEnd; ++itr, ++itrOut)
		{
			if (getDistance(*itr) < m_minDist)
				*itrOut = 255;
			else
				*itrOut = 0;
		}
		return result;
	}
private:
	int getDistance(const cv::Vec3b& color) const
	{
		return abs(color[0] - m_targetColor[0])
			+ abs(color[1] - m_targetColor[1])
			+ abs(color[2] - m_targetColor[2]);
	}

private:
	cv::Vec3b m_targetColor;
	int m_minDist;
};

#endif //_X_COLOR_DETECTOR_HPP
