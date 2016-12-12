#include "stdafx.h"
#include "OpenCVKinect.h"
#include "FingerTracker.h"

using namespace std;

#define FACE_REC 0

#define THRESHOLD 40
#define IMG_THRES 100

#define LH 0
#define HH 20
#define LS 40
#define HS 150
#define LV 60
#define HV 255

char faceDir[100];


//vector<double> prevLength;


const std::string outputDir("D:/Sahid_Development/MeruCabsBookingApplication/data/facerec/New_Faces");

OpenCVKinect::OpenCVKinect(void)
{
}

OpenCVKinect::~OpenCVKinect(void)
{
	delete[] m_streams;
	m_color.destroy();

	OpenNI::shutdown();

	if(!m_haarCascade.empty()) 
	{
		m_haarCascade.~CascadeClassifier();
	}
}

/*void drawKeypad(Mat& inputMat, Mat& outputMat)
{
int count = 0;
for (int i = 0; i < NUM_SQUARES_X; i++)
{
int cnt = count++;
for (int j = 0; j < NUM_SQUARES_Y; j++)
{
int startX = OFFSET_X + SQUARE_SIZE * i;
int startY = OFFSET_Y + SQUARE_SIZE * j;

int endX = startX + SQUARE_SIZE;
int endY = startY + SQUARE_SIZE;

rectangle(inputMat, Point(startX, startY), Point(endX, endY), Scalar(0, 255, 0), 2, 8, 0);

string box_text = format("%d", cnt);
putText(inputMat, box_text, Point(startX + (endX - startX)/2 - 15, startY + (endY - startY)/2 + 15), FONT_HERSHEY_PLAIN, 2.0, CV_RGB(0, 255, 0), 4.0);
cnt = cnt + 3;
}

cvtColor(inputMat, outputMat, CV_BGR2RGB);
}
} */

/*void OpenCVKinect::tectAndDrawContour(Mat& displayFrame, Mat& contourframe) 
{
	vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(contourframe, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	size_t largestContour = findLargestContour(contours);

	cv::drawContours(displayFrame, contours, largestContour, COLOR_RED, 1);

	// Convex hull
	if (!contours.empty() && contourArea(contours[largestContour]) > 5000)
	{
		std::vector<std::vector<cv::Point>> hull(1);
		cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
		////////////////////////////////////////////////////////////////////
		cv::drawContours(displayFrame, hull, 0, COLOR_GREEN, 3);
		////////////////////////////////////////////////////////////////////

		if (hull[0].size() > 2)
		{
			std::vector<int> hullIndexes;
			cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
			std::vector<cv::Vec4i> convexityDefects;
			cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);

			/////////////////////////////////////////////////////////////////
			cv::Moments mo = cv::moments(hull[0]);
			cv::Point cog = cv::Point(mo.m10/mo.m00 , mo.m01/mo.m00);
			/////////////////////////////////////////////////////////////////

			cv::Rect boundingBox = cv::boundingRect(hull[0]);
			cv::rectangle(displayFrame, boundingBox, COLOR_BLUE);
			cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
						std::vector<cv::Point> validPoints;
						double maxLength = 0;
						cv::Point tempFinger;

			//if(detectIfHand(boundingBox)) 
			{
				std::vector<cv::Point> defectPoints;
				for (size_t i = 0; i < convexityDefects.size(); i++)
				{
					//if(convexityDefects[i][3] < 10 * 256)// && convexityDefects[i][3] < 120 * 256) 
					{
						cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
						cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
						cv::Point p3 = contours[largestContour][convexityDefects[i][2]];

						double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
						double inAngle = innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
						double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));

						double lengthh = std::sqrt(std::pow(p1.x - center.x, 2) + std::pow(p1.y - center.y, 2));
						if(lengthh > maxLength)
						{
							//if(inAngle > 100) 
							//	int x = 0;
							tempFinger = p1;
							//validPoints.push_back(p1);
						}

								
					}
				}
			}

			//if(validPoints.size() < 5)
			{
			//for (size_t i = 0; i < validPoints.size(); i++)
					{
						cv::circle(displayFrame, tempFinger, 8, COLOR_GREEN, 2);
		}
			}
	}
	}
			
}*/


bool OpenCVKinect::init()
{
	if(!openDeviceForCapture())
		return false;

	if(m_colorStreamStatus)
	{
		// create a color object
		m_status = m_color.create(m_device, SENSOR_COLOR);
		if(m_status == STATUS_OK)
		{
			m_status = m_color.start();
			if(m_status != STATUS_OK)
			{
				cout << "OpenCVKinect: Couldn't start color stream: " << endl;
				cout << OpenNI::getExtendedError() << endl;
				m_color.destroy();
				return false;
			}
		}
		else
		{
			cout << "OpenCVKinect: Couldn't find color stream: " << endl;
			cout << OpenNI::getExtendedError() << endl;
			return false;
		}
	}

	this->m_streams = new VideoStream*[C_NUM_STREAMS];
	m_streams[C_COLOR_STREAM] = &m_color;

	///////////////////////////////////////////////////
	/*if(!loadFaceCascadeFile()) 
	{
		TRACE("Face Detection not possible !!");
		return false;
	}*/
	///////////////////////////////////////////////////

#if FACE_REC

	initFaceRecognizer();

	//faceMap[0] = "Sachin";
	//faceMap[1] = "George";
	//faceMap[2] = "Sajiv";
	//faceMap[3] = "Arnold";
	//faceMap[4] = "Narendra";

	if(!loadFaceCascadeFile()) 
	{
		TRACE("Face Detection not possible !!");
		return false;
	}

	faceCount = 1;

#endif

	return true;
}

bool OpenCVKinect::openDeviceForCapture()
{
	m_status = STATUS_OK;
	const char* deviceURI = ANY_DEVICE;

	m_status = OpenNI::initialize();

	cout << "After initialization: " << endl; 
	cout << OpenNI::getExtendedError() << endl;

	// open the device
	m_status = m_device.open(deviceURI);

	if(m_status != STATUS_OK)
	{
		cout << "OpenCVKinect: Device open failed: " << endl;
		cout << OpenNI::getExtendedError() << endl;
		OpenNI::shutdown();
		return false;
	}

	return true;
}

void OpenCVKinect::setMode(int inMode)
{
	m_colorStreamStatus = true;
}

bool OpenCVKinect::loadFaceCascadeFile() 
{
	m_haarCascade.load(C_HAAR_CASCADE_FILE);
	if (m_haarCascade.empty()) 
	{
		return false;
	}

	return true;
}

void OpenCVKinect::initFaceRecognizer() 
{
	if(faceRecognizer != nullptr)
	{
		clearFaceStore(); 
	}

	faceRecognizer = new OpenCVFaceRecognition();
	faceRecognizer->readFaceStore();
	faceModel = createFisherFaceRecognizer();

	vector<Mat> images = faceRecognizer->getStoredFaces();
	vector<int> labels = faceRecognizer->getStoredLabels();

	nxtFaceId = images.size()/4;

	sprintf_s(faceDir, sizeof(faceDir), "%s/%d/", outputDir.c_str(), nxtFaceId);
	std::string faceDirStr = faceDir;

	std::wstring faceDirStrW = std::wstring(faceDirStr.begin(), faceDirStr.end());

	if (CreateDirectory(faceDirStrW.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
	}

	faceModel->train(images, labels);
}

vector<Mat> OpenCVKinect::getData()
{
	vector<Mat> returnVec;
	Mat bufferImage;
	bool colorCaptured = false;

	cv::Mat srcHsv;

	if(this->m_colorStreamStatus)
	{
		while(!colorCaptured)
		{
			m_status = OpenNI::waitForAnyStream(m_streams, C_NUM_STREAMS, &m_currentStream, C_STREAM_TIMEOUT);
			if(m_status != STATUS_OK)
			{
				cout << "OpenCVKinect: Unable to wait for streams. Exiting" << endl;
				exit(EXIT_FAILURE);
			}

			m_color.readFrame(&m_colorFrame);
			m_colorImage.create(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3);
			bufferImage.create(m_colorFrame.getHeight(), m_colorFrame.getWidth(), CV_8UC3);
			bufferImage.data = (uchar*)m_colorFrame.getData();

			colorCaptured = true;

#if FACE_REC
			Mat original = bufferImage.clone();
			// Convert the current frame to grayscale:
			Mat gray;
			cvtColor(original, gray, CV_BGR2GRAY);
			//equalizeHist(gray, gray);  

			// Find the faces in the frame:
			vector< Rect_<int> > faces;

			//m_haarCascade.detectMultiScale(gray, faces, C_SCALE_FACTOR, C_NUM_NEIGHBOURS, 
			//							   CV_HAAR_SCALE_IMAGE, Size(100, 100));


			m_haarCascade.detectMultiScale(gray, faces, reject_levels, level_weights, C_SCALE_FACTOR, 
				C_NUM_NEIGHBOURS, CV_HAAR_SCALE_IMAGE, Size(100, 100), Size(150, 150), true);

			for(int i = 0; i < faces.size(); i++) 
			{
				// Process face by face:
				Rect face_i = faces[i];
				// Crop the face from the image. So simple with OpenCV C++:
				Mat face = gray(face_i);

				Mat face_resized;
				resize(face, face_resized, Size(faceRecognizer->getFaceImageWidth(), 
					faceRecognizer->getFaceImageHeight()),
					1.0, 1.0, INTER_CUBIC);

				equalizeHist(face_resized, face_resized);

				//////////////////////////////////////////////////////////
				if(faceCount < 5)
				{
					char absImgPath[100];
					sprintf_s(absImgPath, sizeof(absImgPath), "%s%d.jpg", faceDir, faceCount);
					imwrite(absImgPath, face_resized);

					char inputLine[100];
					sprintf_s(inputLine, sizeof(inputLine), "%s;%d", absImgPath, nxtFaceId);
					faceRecognizer->writeToFaceStore(inputLine);
					faceCount++;
				}
				//////////////////////////////////////////////////////////

				// Now perform the prediction, see how easy that is:
				double confidence = 0.0;
				int prediction = -1;
				// faceModel->predict(face_resized, prediction, confidence);

				prediction = faceModel->predict(face_resized);  //sn

				//int prediction = faceModel->predict(face_resized);

				// And finally write all we've found out to the original image!
				// First of all draw a green rectangle around the detected face:
				rectangle(original, face_i, CV_RGB(0, 255, 0), 1);

				// Create the text we will annotate the box with:
				// string box_text = format("Prediction = %s", faceMap[prediction].c_str()); //Confidence = %d", prediction, confidence);

				if(reject_levels.size() != 0)
				{
					if(reject_levels[i] > THRESHOLD)
					{
						string box_text = format("Prediction = %d ### %d", prediction, reject_levels[i]);

						// Calculate the position for annotated text (make sure we don't
						// put illegal values in there):
						int pos_x = std::max(face_i.tl().x - 10, 0);
						int pos_y = std::max(face_i.tl().y - 10, 0);
						// And now put it into the image:
						putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
					}
				}
			}

			cvtColor(original, m_colorImage, CV_BGR2RGB);
#else

			FingerTracker *fingerTracker = new FingerTracker();

			cvtColor(bufferImage, m_colorImage, CV_BGR2RGB);

			cv::cvtColor(m_colorImage, srcHsv, CV_BGR2HSV);
			cv::inRange(srcHsv, cv::Scalar(LH, LS, LV), cv::Scalar(HH, HS, HV), srcHsv);

			int blurSize = 5;
			cv::medianBlur(srcHsv, srcHsv, blurSize);
			cv::dilate(srcHsv, srcHsv, getStructuringElement()); 

			fingerTracker->drawKeypad(m_colorImage, m_colorImage);
			//fingerTracker->detectAndDrawContours(m_colorImage, srcHsv);
			fingerTracker->detectAndDrawContour(m_colorImage, srcHsv);

			delete fingerTracker;
		}
	}

#endif

	returnVec.push_back(m_colorImage);
	m_colorFrame.release();
	bufferImage.release();
	m_colorImage.release();
	return returnVec;
}

void OpenCVKinect::imageMatrixResize(const Rect& face, Mat& resized)
{
	Mat matrix;
	cv::resize(matrix(face), resized, Size(faceRecognizer->getFaceImageWidth(), 
		faceRecognizer->getFaceImageHeight()), 
		1.0, 1.0, INTER_CUBIC);
}

cv::Mat OpenCVKinect::getStructuringElement()
{
	int elementSize = 5;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), 
												cv::Point(elementSize, elementSize));
	return element;
}

int OpenCVKinect::convertMatToImage(Mat *mat, CImage &img)
{
	if(!mat || mat->empty())
		return -1;

	int nBPP = mat->channels() * 8;
	img.Create(mat->cols, mat->rows, nBPP);

	if(nBPP == 8)
	{
		static RGBQUAD pRGB[256];
		for (int i = 0; i < 256; i++)
		{
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
		}

		img.SetColorTable(0, 256, pRGB);
	}

	uchar* psrc = mat->data;
	uchar* pdst = (uchar*) img.GetBits();
	int imgPitch = img.GetPitch();

	for(int y = 0; y < mat->rows; y++)
	{
		//mat->step is incorrect for those images created by roi (sub-images!)
		memcpy(pdst, psrc, mat->cols*mat->channels());
		psrc += mat->step;
		pdst += imgPitch;
	}

	return 0;
}

void OpenCVKinect::getResizedImage(CDC *dc, CPoint *pt, CImage img, CBitmap &bm)
{
	CDC mDC;
	mDC.CreateCompatibleDC(dc);

	bm.CreateCompatibleBitmap(dc, pt->x, pt->y);

	CBitmap *pob = mDC.SelectObject(&bm);
	mDC.SetStretchBltMode(HALFTONE);
	img.StretchBlt(mDC.m_hDC, 0, 0, pt->x, pt->y, 0, 0, img.GetWidth(), img.GetHeight(), SRCCOPY);
	mDC.SelectObject(pob);
}

void OpenCVKinect::clearFaceStore() 
{
	delete faceRecognizer;
}
