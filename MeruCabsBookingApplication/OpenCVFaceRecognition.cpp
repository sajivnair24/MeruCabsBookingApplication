#include "stdafx.h"
#include "OpenCVFaceRecognition.h"

#include <ios>
#include <fstream>

OpenCVFaceRecognition::OpenCVFaceRecognition(void){}

OpenCVFaceRecognition::~OpenCVFaceRecognition(void){}

void readFaceCSVFile(vector<Mat>& images, vector<int>& labels) 
{
	char separator = ';';
	ifstream inFile(facesCSVFile.c_str(), ifstream::in);

	if (!inFile) 
	{
		string errMsg = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, errMsg);
	}

	string line, filePath, classLabel;
	while (getline(inFile, line)) 
	{
		stringstream lines(line);
		getline(lines, filePath, separator);
		getline(lines, classLabel);

		if(!filePath.empty() && !classLabel.empty()) 
		{
			images.push_back(imread(filePath, 0));
			labels.push_back(atoi(classLabel.c_str()));
		}
	}
}

void writeToCSVFile(string line)
{
	std::ofstream log(facesCSVFile, std::ios_base::app | std::ios_base::out);
	log << "\n" << line ;
}

void OpenCVFaceRecognition::readFaceStore() 
{
    try 
	{
        readFaceCSVFile(images, labels);
    } 
	catch (cv::Exception& e) 
	{
        cerr << "Couldn't open face store file. Reason: " << e.msg << endl;
    }
}

void OpenCVFaceRecognition::writeToFaceStore(string line) 
{
	writeToCSVFile(line);
}

vector<Mat> OpenCVFaceRecognition::getStoredFaces()
{
	return images;
}

vector<int> OpenCVFaceRecognition::getStoredLabels()
{
	return labels;
}

int OpenCVFaceRecognition::getFaceImageWidth()
{
	return images[0].cols;
}

int OpenCVFaceRecognition::getFaceImageHeight()
{
	return images[0].rows;
}