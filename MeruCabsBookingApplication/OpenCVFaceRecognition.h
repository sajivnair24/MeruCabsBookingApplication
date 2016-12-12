#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

const string facesCSVFile = "../data/facerec/faces.csv";

class OpenCVFaceRecognition
{
	vector<Mat> images;
	vector<int> labels;

public:
	OpenCVFaceRecognition(void);
	~OpenCVFaceRecognition(void);

	void readFaceStore(); 
	void writeToFaceStore(string line);

	vector<Mat> getStoredFaces();
	vector<int> getStoredLabels();

	int getFaceImageWidth();
	int getFaceImageHeight();
};

