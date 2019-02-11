#ifndef _X_MAP_HPP
#define _X_MAP_HPP

#include "XPenCV.h"

#ifdef X_USE_QT
#include <QImage>
#endif 

class XMat : public cv::Mat
{
public:
	XMat() :cv::Mat(){}
	XMat(const cv::Mat& mat) :cv::Mat(mat){}
	XMat(int _rows, int _cols, int _type, void* _data, size_t _step)
		:cv::Mat(_rows, _cols, _type, _data, _step){}

	cv::vector<cv::Mat> hist()const
	{
		cv::Mat image = *this;
		cv::vector<cv::Mat> histImgs;
		cv::vector<cv::Mat> mv;
		cv::split(image, mv);

		for (auto& m : mv)
		{
			histImgs.push_back(hist(m));
		}

		return histImgs;
	}

	void reduceColor(int div = 64)
	{
		int nl = this->rows;
		int nc = this->cols;
		int ch = this->channels();

		if (isContinuous())
		{
			nc = nc*nl;
			nl = 1;
		}
		int n = static_cast<int>(log(static_cast<double>(div)) / log(2.0));
		uchar mask = 0xFF << n;
		for (int j = 0; j < nl; ++j)
		{
			uchar* data = ptr<uchar>(j);
			for (int i = 0; i < nc; ++i)
			{
				for (int chn = 0; chn < ch; ++chn)
				{
					*data++ = *data&mask + div / 2;
				}
			}
		}
	}
	void sharpen()
	{
		cv::Mat mat;
		sharpen(*this, mat);
		*this = mat;
	}

	void drawRotatedRect(const cv::RotatedRect& rotatedRect, const cv::Scalar& color)
	{
		cv::Point2f pt[4];
		rotatedRect.points(pt);
		for (int i = 0; i < 4; ++i)
		{
			cv::line(*this, pt[i], pt[(i + 1) % 4], color);
		}
	}

	XMat extract(const cv::RotatedRect& roi)
	{
		auto src = (*this)(roi.boundingRect()).clone();
		cv::Mat dst;
		cv::warpAffine(src, dst,
			cv::getRotationMatrix2D(cv::Point2f(src.size().width / 2.0f, src.size().height / 2.0f), roi.angle, 1.0f ), 
			src.size());

		auto x = (src.size().width - roi.size.width) / 2.0;
		auto y = (src.size().height - roi.size.height) / 2.0;
		return dst(cv::Rect(cv::Point((int)x,(int)y), roi.size));
	}

	static void sharpen(const cv::Mat& image, cv::Mat& result)
	{
		cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
		kernel.at<float>(1, 1) = 5;
		kernel.at<float>(0, 1) = -1;
		kernel.at<float>(2, 1) = -1;
		kernel.at<float>(1, 0) = -1;
		kernel.at<float>(1, 2) = -1;
		cv::filter2D(image, result, image.depth(), kernel);
	}

#ifdef X_USE_QT
	static XMat fromQImage(const QImage& image)
	{
		XMat mat;
		switch (image.format())
		{
		case QImage::Format_ARGB32:
		case QImage::Format_RGB32:
		case QImage::Format_ARGB32_Premultiplied:
			mat = XMat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
			break;
		case QImage::Format_RGB888:
			mat = XMat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
			cv::cvtColor(mat, mat, CV_BGR2RGB);
			break;
		case QImage::Format_Indexed8:
			mat = XMat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
			break;
		}
		return mat;
	}
	QImage toQImage()const
	{
		if (type() == CV_8UC4)
		{
			return QImage((const uchar*)data, cols, rows, step, QImage::Format_RGB32).copy();
		}
		if (type() == CV_8UC3)
		{
			return QImage((const uchar*)data, cols, rows, step, QImage::Format_RGB888).rgbSwapped();
		}
		if (type() == CV_8UC1)
		{
			QImage image(cols, rows, QImage::Format_Indexed8);
			image.setColorCount(256);
			for (int i = 0; i < 256; i++)
			{
				image.setColor(i, qRgb(i, i, i));
			}

			uchar *pSrc = data;
			for (int row = 0; row < rows; row++)
			{
				uchar *pDest = image.scanLine(row);
				memcpy(pDest, pSrc, cols);
				pSrc += step;
			}
			return image;
		}

		assert(false);
		return QImage();
	}
#endif

	static cv::MatND getHist(const cv::Mat& image, int size = 256)
	{
		cv::MatND dstHist;
		int channels[] = { 0 };
		int histSize[] = { size };
		float range[] = { 0.0f, size - 1.0f };
		const float* ranges[] = { range };
		cv::calcHist(&image, 1, channels, cv::Mat(), dstHist, 1, histSize, ranges);
		return dstHist;
	}


private:
	cv::Mat hist(const cv::Mat& image, int size=256)const
	{
		cv::MatND dstHist = getHist(image, size);
		cv::Mat dstImg(cv::Size(size,size), CV_8U, cv::Scalar(0));
		double minVal = 0;
		double maxVal = 0;
		cv::minMaxLoc(dstHist, &minVal, &maxVal);
		int hpt = cv::saturate_cast<int>(0.9*size);
		for (int i = 0; i < size; ++i)
		{
			float binVal = dstHist.at<float>(i);
			int realVal = cv::saturate_cast<int>(binVal*hpt / maxVal);
			cv::line(dstImg, cv::Point(i, size), cv::Point(i, size- realVal), cv::Scalar::all(255));
		}
		return dstImg;
	}
};

#endif //_X_MAP_HPP
