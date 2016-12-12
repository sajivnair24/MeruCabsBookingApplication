#pragma once
#include <opencv\cv.h>

using namespace cv;

class FingerTracker
{
public:
	FingerTracker(void);
	~FingerTracker(void);

	void drawKeypad(Mat& inputMat, Mat& outputMat);

	void findContours(Mat& inputMat, std::vector<std::vector<cv::Point>>& contours);
	void drawContours(Mat& inputMat, std::vector<std::vector<cv::Point>>& contours, size_t contourIndex);

	cv::Moments getMomentOfInertia(std::vector<std::vector<cv::Point>>& convexHull);
	cv::Rect getBoundingBox(std::vector<std::vector<cv::Point>>& convexHull);

	cv::Point getCentreOfGravity(cv::Moments& moment);
	cv::Point getCentreOfBoundingBox(cv::Rect& boundingBox);

	void getFingerTipsAndDefects(std::vector<cv::Vec4i>& convexityDefects, 
								 std::vector<std::vector<cv::Point>>& contours,
								 size_t contourIndex, cv::Rect& boundingBox,
								 vector<cv::Point>& fingerTips,
								 vector<cv::Point>& defectTips);
	
	void detectAndDrawContours(Mat& displayFrame, Mat& contourFrame);
	void detectAndDrawContour(Mat& displayFrame, Mat& contourFrame);
};

