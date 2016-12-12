
#pragma once
#include <openni\OpenNI.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "OpenCVFaceRecognition.h"

#include <vector>
#include <map>

#define C_COLOR_STREAM 0
#define C_NUM_STREAMS 1
#define C_MODE_COLOR 0x01
#define C_MODE_DEPTH 0x02
#define C_STREAM_TIMEOUT -1//2000

#define C_SCALE_FACTOR 1.1
#define C_NUM_NEIGHBOURS 3
#define C_HAAR_CASCADE_FILE "../data/haarcascade/haarcascade_frontalface_default.xml"

using namespace cv;
using namespace openni;

class OpenCVKinect
{
	Status m_status;
	Device m_device;
	VideoStream m_color, **m_streams;
	VideoFrameRef m_colorFrame;
	int m_currentStream;
	Mat m_colorImage;
	bool m_colorStreamStatus;

	CascadeClassifier m_haarCascade;

	OpenCVFaceRecognition *faceRecognizer;
	Ptr<FaceRecognizer> faceModel;

	map<int, string> faceMap;

	vector<int> reject_levels;
    vector<double> level_weights;

public:
	OpenCVKinect(void);
	~OpenCVKinect(void);

	int faceCount;
	int nxtFaceId;

	bool init();
	bool openDeviceForCapture();
	void setMode(int inMode);
	bool loadFaceCascadeFile();
	void initFaceRecognizer();
	vector<Mat> getData();

	int convertMatToImage(Mat *mat, CImage &img);
	void getResizedImage(CDC *dc, CPoint *pt, CImage img, CBitmap &bm);

	cv::Mat getStructuringElement();

	void clearFaceStore();

	void imageMatrixResize(const Rect& face, Mat& resized);
};

