#include "stdafx.h"
#include "FingerTracker.h"

#define IN_ANGLE_MIN 200
//#define IN_ANGLE_MAX 300
#define IN_ANGLE_MAX 250
#define ANGLE_MIN 180
#define ANGLE_MAX 359
#define LENGTH_MIN 10
#define LENGTH_MAX 80 

#define SQUARE_GAP 25
#define SQUARE_SIZE 75

#define NUM_SQUARES_X 3
#define NUM_SQUARES_Y 3

#define MAX_CONTOUR_AREA 5000

const cv::Scalar COLOR_RED (0, 0, 255);
const cv::Scalar COLOR_GREEN (0, 255, 0);
const cv::Scalar COLOR_BLUE (255, 0, 0);

vector<cv::Point> btnPnts;
vector<cv::string> btnTxts;
vector<float> prevPnts;

int avgX = 0;
int avgY = 0;

/*struct Person {
	char name[20];
	int age;
};

int getPersonAge(Person *person)
{
	return person->age;
}

char *getPersonName(Person *person)
{
	return person->name;
}*/

FingerTracker::FingerTracker(void)
{
/*	Person *person = new Person();
	person->age = 20;
	int agg = getPersonAge(person);
	strcpy(person->name, "Hiiiiiiiiiii & Byyeeeeeeeeeeeeeeeeeeeeee");
	char *a = getPersonName(person);
	std::string s(a);
	*/
}

FingerTracker::~FingerTracker(void)
{
}

float innerAngle(float px1, float py1, float px2, float py2, float cx1, float cy1)
{
	float dist1 = std::sqrt((px1-cx1)*(px1-cx1) + (py1-cy1)*(py1-cy1));
	float dist2 = std::sqrt((px2-cx1)*(px2-cx1) + (py2-cy1)*(py2-cy1));

	float Ax, Ay;
	float Bx, By;
	float Cx, Cy;

	Cx = cx1;
	Cy = cy1;
	if(dist1 < dist2)
	{
		Bx = px1;
		By = py1;
		Ax = px2;
		Ay = py2;

	} else {
		Bx = px2;
		By = py2;
		Ax = px1;
		Ay = py1;
	}

	float Q1 = Cx - Ax;
	float Q2 = Cy - Ay;
	float P1 = Bx - Ax;
	float P2 = By - Ay;


	float A = std::acos( (P1*Q1 + P2*Q2) / ( std::sqrt(P1*P1+P2*P2) * std::sqrt(Q1*Q1+Q2*Q2) ) );

	A = A*180/CV_PI;

	return A;
}

float distanceP2P(const Point& a, const Point& b)
{
	float d = sqrt(fabs(pow(a.x-b.x,2) + pow(a.y-b.y,2)));  
	return d;
}

bool detectIfHand(const cv::Rect& boundingRect)
{
	bool isHand = true;

	double h = boundingRect.height; 
	double w = boundingRect.width;

	if(h == 0 || w == 0)
	{
		isHand = false;
	}
	else if(h/w < 1.8 )//|| w/h > 1.5)
	{
		isHand = false;	
	}
	else if(boundingRect.x < 20)
	{
		isHand = false;	
	}	

	return isHand;
}

void getKeyPadButtonRects(vector<cv::Rect>& btnRect)
{
	cv::Rect roi;

	for (size_t i = 0; i < btnPnts.size(); i++) 
	{
		roi = cv::Rect(btnPnts[i].x, btnPnts[i].y, btnPnts[i].x + SQUARE_SIZE, btnPnts[i].y + SQUARE_SIZE);
		btnRect.push_back(roi);
	}
}

size_t findLargestContour(const std::vector<std::vector<cv::Point>>& contours)
{
	size_t largestContour = 0;

	for (size_t i = 1; i < contours.size(); i++)
	{
		if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour]))
		{
			largestContour = i;
		}
	}

	return largestContour;
}

void FingerTracker::drawKeypad(Mat& inputMat, Mat& outputMat)
{
	int count = 0;
	int offsetx = 75;
	int offsety = 40;

	btnPnts.clear();
	btnTxts.clear();

	for (int i = 0; i < NUM_SQUARES_X; i++)
	{
		int cnt = count++;
		offsetx += SQUARE_SIZE + SQUARE_GAP;

		for (int j = 0; j < NUM_SQUARES_Y; j++)
		{
			int startX = offsetx;
			int startY = offsety + j * (SQUARE_SIZE + SQUARE_GAP);

			int endX = startX + SQUARE_SIZE;
			int endY = startY + SQUARE_SIZE;

			cv::rectangle(inputMat, Point(startX, startY), Point(endX, endY), COLOR_GREEN, 2, 8, 0);
			btnPnts.push_back(Point(startX, startY));

			string box_text = format("%d", cnt);
			btnTxts.push_back(box_text);
			putText(inputMat, box_text, Point(startX + (endX - startX)/2 - 15, startY + (endY - startY)/2 + 15), FONT_HERSHEY_PLAIN, 3, CV_RGB(0, 255, 0), 4);
			cnt = cnt + 3;
		}
	}

	//cvtColor(inputMat, outputMat, CV_BGR2RGB);
}

void FingerTracker::findContours(Mat& inputMat, std::vector<std::vector<cv::Point>>& contours)
{
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(inputMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
}

void FingerTracker::drawContours(Mat& inputMat, std::vector<std::vector<cv::Point>>& contours, size_t contourIndex)
{
	cv::drawContours(inputMat, contours, contourIndex, COLOR_RED, 1);
}

cv::Moments FingerTracker::getMomentOfInertia(std::vector<std::vector<cv::Point>>& convexHull)
{
	return cv::moments(convexHull[0]);
}

cv::Point FingerTracker::getCentreOfGravity(cv::Moments& moment)
{
	return cv::Point(moment.m10/moment.m00 , moment.m01/moment.m00);
}

cv::Rect FingerTracker::getBoundingBox(std::vector<std::vector<cv::Point>>& convexHull)
{
	return cv::boundingRect(convexHull[0]);
}

cv::Point FingerTracker::getCentreOfBoundingBox(cv::Rect& boundingBox)
{
	return cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
}

int calculateTilt(cv::Moments& moment)
{
  double diff = moment.m20 - moment.m02;

  if (diff == 0) 
  {
    if (moment.m11 == 0)
      return 0;
    else if (moment.m11 > 0)
      return 45;
    else   // m11 < 0
      return -45;
  }

  //std::atan2(center.y - fPts[0].y, center.x - fPts[0].x) * 180 / CV_PI;
  double theta = 0.5 * std::atan2(2 * moment.m11, diff);
  int tilt = (int) theta * 180 / CV_PI;//Math.round( Math.toDegrees(theta));

  if ((diff > 0) && (moment.m11 == 0))
    return 0;
  else if ((diff < 0) && (moment.m11 == 0))
    return -90;
  else if ((diff > 0) && (moment.m11 > 0))  // 0 to 45 degrees
    return tilt;
  else if ((diff > 0) && (moment.m11 < 0))  // -45 to 0
    return (180 + tilt);   // change to counter-clockwise angle
  else if ((diff < 0) && (moment.m11 > 0))   // 45 to 90
    return tilt;
  else if ((diff < 0) && (moment.m11 < 0))   // -90 to -45
    return (180 + tilt);  // change to counter-clockwise angle

  //System.out.println("Error in moments for tilt angle");
  return 0;
} 

void FingerTracker::getFingerTipsAndDefects(std::vector<cv::Vec4i>& convexityDefects, 
											std::vector<std::vector<cv::Point>>& contours,
											size_t contourIndex, cv::Rect& boundingBox,
											vector<cv::Point>& fingerTips,
											vector<cv::Point>& defectTips)
{
	cv::Point fPts[3];
	cv::Point center = getCentreOfBoundingBox(boundingBox);

	for (size_t i = 0; i < convexityDefects.size(); i++)
	{
		//if(convexityDefects[i][3] > 20 * 256 && convexityDefects[i][3] < 120 * 256) 
		{
			fPts[0] = contours[contourIndex][convexityDefects[i][0]];
			fPts[1] = contours[contourIndex][convexityDefects[i][1]];
			fPts[2] = contours[contourIndex][convexityDefects[i][2]];

			double angle = std::atan2(center.y - fPts[0].y, center.x - fPts[0].x) * 180 / CV_PI;
			double inAngle = innerAngle(fPts[0].x, fPts[0].y, fPts[1].x, fPts[1].y, fPts[2].x, fPts[2].y);
			double length = std::sqrt(std::pow(fPts[0].x - fPts[2].x, 2) + std::pow(fPts[0].y - fPts[2].y, 2));

			if (angle > ANGLE_MIN - 180 && angle < ANGLE_MAX - 180 && inAngle > IN_ANGLE_MIN - 180 && inAngle < IN_ANGLE_MAX - 180 && length > LENGTH_MIN / 100.0 * boundingBox.height && length < LENGTH_MAX / 100.0 * boundingBox.height)
			{
				fingerTips.push_back(fPts[0]);
				defectTips.push_back(fPts[2]);
			} 
		}
	}
}

enum FingerName 
{
   LITTLE, RING, MIDDLE, INDEX, THUMB, UNKNOWN
};

const int MIN_THUMB = 120;
const int MAX_THUMB = 200;

const int MIN_INDEX = 60;
const int MAX_INDEX = 120;

int angleToCOG(Point tipPt, Point cogPt, int contourAxisAngle);

void labelThumbIndex(vector<cv::Point>& fingerTips,
                     vector<int>& nms,
					 cv::Point& cog,
					 int contourAxisAngle)
{
	boolean foundThumb = false;
	boolean foundIndex = false;

	int i = fingerTips.size() - 1;

	while ((i >= 0)) 
	{
		int angle = angleToCOG(fingerTips[i], cog, contourAxisAngle);

		// check for thumb
		if ((angle <= MAX_THUMB) && (angle>MIN_THUMB) && !foundThumb) 
		{
			nms.push_back(THUMB);
			foundThumb = true;
		}

		// check for index
		if ((angle <= MAX_INDEX) && (angle > MIN_INDEX) && !foundIndex) 
		{
			nms.push_back(INDEX);
			foundIndex = true;
		}

		i--;
	}
}  // end of labelThumbIndex()

int angleToCOG(Point tipPt, Point cogPt, int contourAxisAngle)
{
	int yOffset = cogPt.y - tipPt.y;    // make y positive up screen
	int xOffset = tipPt.x - cogPt.x;
	double theta = std::atan2(yOffset, xOffset);
	int angleTip = (int) theta * 180 / CV_PI;
	 return angleTip + (90 - contourAxisAngle);
           // this ensures that the hand is orientated straight up
}  // end of angleToCOG()

void FingerTracker::detectAndDrawContour(Mat& displayFrame, Mat& contourFrame) 
{
	std::vector<std::vector<cv::Point>> contours;
	findContours(contourFrame, contours);

	size_t largestContour = findLargestContour(contours);
	drawContours(displayFrame, contours, largestContour);

	// Convex hull
	// if (!contours.empty() && contourArea(contours[largestContour]) > MAX_CONTOUR_AREA)
	if (contours.empty() || contourArea(contours[largestContour]) < MAX_CONTOUR_AREA)
	{
		return;
	}

	std::vector<std::vector<cv::Point>> hull(1);
	cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
	////////////////////////////////////////////////////////////////////
	//cv::drawContours(displayFrame, hull, 0, COLOR_GREEN, 3);
	////////////////////////////////////////////////////////////////////

	//if (hull[0].size() > 2)
	if (hull[0].size() < 3)
	{
		return;
	}

	std::vector<int> hullIndexes;
	cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
	std::vector<cv::Vec4i> convexityDefects;
	cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);

	//cv::Point cog = getCentreOfGravity(getMomentOfInertia(hull));

	cv::Rect boundingBox = getBoundingBox(hull);
	cv::rectangle(displayFrame, boundingBox, COLOR_BLUE);
	cv::Point center = getCentreOfBoundingBox(boundingBox);

	std::vector<cv::Point> validPoints;

	//if(detectIfHand(boundingBox)) 
	//{
	std::vector<cv::Point> defectPoints;

	getFingerTipsAndDefects(convexityDefects, contours, largestContour, boundingBox, validPoints, defectPoints);

	////////////////////////////////////////////////////////////////////////

	cv::Point cog = getCentreOfGravity(getMomentOfInertia(hull));
	int contourAxisAngle = calculateTilt(getMomentOfInertia(hull));

	if (validPoints.size() > 0) 
	{
		int yTotal = 0;
		for(Point pt : validPoints)
			yTotal += pt.y;
		int avgYFinger = yTotal/validPoints.size();
		if (avgYFinger > cog.y)   // fingers below COG
			contourAxisAngle += 180;
	}

	contourAxisAngle = 180 - contourAxisAngle;  

	vector<int> namedFingers;
	for (int i = 0; i < validPoints.size(); i++)
		namedFingers.push_back(UNKNOWN);

	labelThumbIndex(validPoints, namedFingers, cog, contourAxisAngle);
	int x = 0;
	//labelUnknowns(namedFingers);
	
	////////////////////////////////////////////////////////////////////////

}

void FingerTracker::detectAndDrawContours(Mat& displayFrame, Mat& contourFrame) 
{
	vector<cv::Rect> btnRect;
	std::vector<std::vector<cv::Point>> contours;

	getKeyPadButtonRects(btnRect);

	findContours(contourFrame, contours);

	size_t largestContour = findLargestContour(contours);
	drawContours(displayFrame, contours, largestContour);

	// Convex hull
	// if (!contours.empty() && contourArea(contours[largestContour]) > MAX_CONTOUR_AREA)
	if (contours.empty() || contourArea(contours[largestContour]) < MAX_CONTOUR_AREA)
	{
		return;
	}

	std::vector<std::vector<cv::Point>> hull(1);
	cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
	////////////////////////////////////////////////////////////////////
	//cv::drawContours(displayFrame, hull, 0, COLOR_GREEN, 3);
	////////////////////////////////////////////////////////////////////

	//if (hull[0].size() > 2)
	if (hull[0].size() < 3)
	{
		return;
	}

	std::vector<int> hullIndexes;
	cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
	std::vector<cv::Vec4i> convexityDefects;
	cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);

	//cv::Point cog = getCentreOfGravity(getMomentOfInertia(hull));

	cv::Rect boundingBox = getBoundingBox(hull);
	cv::rectangle(displayFrame, boundingBox, COLOR_BLUE);
	cv::Point center = getCentreOfBoundingBox(boundingBox);

	std::vector<cv::Point> validPoints;

	//if(detectIfHand(boundingBox)) 
	//{
	std::vector<cv::Point> defectPoints;

	getFingerTipsAndDefects(convexityDefects, contours, largestContour, boundingBox, validPoints, defectPoints);

	////////////////////////////////////////////////////////////////////////

	cv::Point cog = getCentreOfGravity(getMomentOfInertia(hull));
	int contourAxisAngle = calculateTilt(getMomentOfInertia(hull));

	if (validPoints.size() > 0) 
	{
		int yTotal = 0;
		for(Point pt : validPoints)
			yTotal += pt.y;
		int avgYFinger = yTotal/validPoints.size();
		if (avgYFinger > cog.y)   // fingers below COG
			contourAxisAngle += 180;
	}

	contourAxisAngle = 180 - contourAxisAngle;  
	
	////////////////////////////////////////////////////////////////////////

	size_t nFingers = validPoints.size();

	if(nFingers == 5 || nFingers == 4)
	{
		//cv::drawContours(displayFrame, hull, 0, COLOR_GREEN, 3);

		int sumX = 0;
		int sumY = 0;
		size_t k = 0;

		bool inRect = false;

		for (size_t i = 0; i < nFingers; i++)
		{
			//cv::circle(displayFrame, validPoints[i], 8, COLOR_GREEN, 2);

			for (size_t j = 0; j < btnRect.size(); j++) 
			{
				if(prevPnts.size() == nFingers) 
				{
					if(distanceP2P(cv::Point(avgX, avgY), validPoints[i]) < (prevPnts[i] - (0.20 * prevPnts[i])))
					{
						if (btnRect[j].contains(validPoints[i]))
						{
							inRect = true;
							if(k < j) k = j;
						}
					}
				}
			}

			sumX += defectPoints[i].x;
			sumY += defectPoints[i].y;
		}

		avgX = sumX/nFingers;
		avgY = sumY/nFingers;

		if(k >= 0 && inRect) 
		{					
			cv::rectangle(displayFrame, Point(btnRect[k].x, btnRect[k].y), Point(btnRect[k].x + SQUARE_SIZE, btnRect[k].y + SQUARE_SIZE), COLOR_GREEN, -1, 8, 0);
			putText(displayFrame, btnTxts[k], Point(500, 100), FONT_HERSHEY_PLAIN, 8, CV_RGB(0, 255, 255), 12);
		}

		//cv::circle(displayFrame, cv::Point(avgX, avgY), 8, COLOR_GREEN, 2);
		prevPnts.clear();

		for (size_t i = 0; i < validPoints.size(); i++)
		{
			//cv::line(displayFrame, cv::Point(avgX, avgY), validPoints[i], COLOR_BLUE, 2);
			prevPnts.push_back(distanceP2P(cv::Point(avgX, avgY), validPoints[i]));
		}
	}
	//}
	//}
	//}
} 

