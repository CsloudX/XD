#ifndef _X_HISTOGRAM_HPP
#define _X_HISTOGRAM_HPP

class XHistogram
{
public:
	XHistogram()
	{
		m_histSize[0] = m_histSize[1] = m_histSize[2]=  256;
		m_hRanges[0] = 0;
		m_hRanges[1] = 255;
		m_ranges[0] = m_ranges[1] = m_ranges[2] = m_hRanges;
		m_channels[0] = 0;
		m_channels[1] = 1;
		m_channels[2] = 2;
	}

	cv::Mat getHistogramImage(const cv::Mat& image)
	{
		cv::MatND hist = getHistogram(image);
		double maxVal = 0;
		double minVal = 0;
		cv::minMaxLoc(hist, &minVal, &maxVal);
		cv::Mat histImg(m_histSize[0], m_histSize[0], CV_8U, cv::Scalar(255));
		int hpt = static_cast<int>(m_histSize[0]);
		for (int h = 0; h < m_histSize[0]; ++h)
		{
			float binVal = hist.at<float>(h);
			int intersity = static_cast<int>(binVal*hpt / maxVal);
			std::cout << binVal<<std::endl;
			cv::line(histImg, cv::Point(h, m_histSize[0]),
				cv::Point(h, m_histSize[0] - intersity),
				cv::Scalar::all(0));
		}
		return histImg;
	}

	cv::MatND getHistogram(const cv::Mat& image)
	{
		cv::MatND hist;
		cv::calcHist(&image,
			1, m_channels, cv::Mat(), hist, 
			image.channels()>1?3:1, 
			m_histSize, m_ranges);
		return hist;
	}

private:
	int m_histSize[3];
	float m_hRanges[2];
	const float* m_ranges[3];
	int m_channels[3];
};

#endif //_X_HISTOGRAM_HPP
