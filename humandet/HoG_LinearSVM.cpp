#include "HoG_LinearSVM.h"


Class_HoG_LinearSVM::Class_HoG_LinearSVM(Size win_size) 
	: HOGDescriptor(win_size, Size(16,16), Size(8,8), Size(8,8), 9, 1, -1, HOGDescriptor::L2Hys, 0.2, true, HOGDescriptor::DEFAULT_NLEVELS)
{
	vector<float> svmCoeffs;
	if(win_size==Size(48,96))
		svmCoeffs = HOGDescriptor::getDaimlerPeopleDetector();
	else if(win_size==Size(64,128))
		svmCoeffs = HOGDescriptor::getDefaultPeopleDetector();
	else
		svmCoeffs = HOGDescriptor::getDefaultPeopleDetector();
	this->setSVMDetector(svmCoeffs);
}
/*
HOGDescriptor() : 
	winSize(64,128), blockSize(16,16), blockStride(8,8), cellSize(8,8), 
	nbins(9), derivAperture(1), winSigma(-1), histogramNormType(HOGDescriptor::L2Hys), 
	L2HysThreshold(0.2), gammaCorrection(true),	nlevels(HOGDescriptor::DEFAULT_NLEVELS)
Parameters:	
	win_size �C Detection window size. Align to block size and block stride.
	block_size �C Block size in pixels. Align to cell size. Only (16,16) is supported for now.
	block_stride �C Block stride. It must be a multiple of cell size.
	cell_size �C Cell size. Only (8, 8) is supported for now.
	nbins �C Number of bins. Only 9 bins per cell are supported for now.
	win_sigma �C Gaussian smoothing window parameter.
	threshold_L2hys �C L2-Hys normalization method shrinkage.
	gamma_correction �C Flag to specify whether the gamma correction preprocessing is required or not.
	nlevels �C Maximum number of detection window increases.
*/


Class_HoG_LinearSVM::~Class_HoG_LinearSVM(){
}


// 'im' can be either BRG or gray image
void Class_HoG_LinearSVM::myDetect(const Mat &im, vector<ObjectDetection> &dets)
{
	double hitThresh = 0;
	double scale = 1.05;
	double finalThresh = 0;
	bool useMeanshift = true;

	vector<Rect> foundLocations;
	vector<double> foundWeights;
	this->detectMultiScale(im, foundLocations, foundWeights, hitThresh, Size(), Size(), 
							scale, finalThresh, useMeanshift);	// see below for detailed description of 'detectMultiScale' function
	
	dets.clear();
	ObjectDetection tmpdet;
	int num = foundLocations.size();	
	for(int i=0; i<num; ++i)	// Need any post-processing? like NMS?
	{
		tmpdet.rect = foundLocations[i];
		tmpdet.score = (float)foundWeights[i];
		dets.push_back(tmpdet);
	}
}
/*
void detectMultiScale(const Mat& img, CV_OUT vector<Rect>& foundLocations, CV_OUT vector<double>& foundWeights, 
					double hitThreshold=0, Size winStride=Size(), Size padding=Size(), double scale=1.05,
					double finalThreshold=2.0, bool useMeanshiftGrouping = false) const;
Parameters:	
	img �C Source image. See gpu::HOGDescriptor::detect() for type limitations.
	found_locations �C Detected objects boundaries.
	hit_threshold �C Threshold for the distance between features and SVM classifying plane. See gpu::HOGDescriptor::detect() for details.
	win_stride �C Window stride. It must be a multiple of block stride.
	padding �C Mock parameter to keep the CPU interface compatibility. It must be (0,0).
	scale0 �C Coefficient of the detection window increase.
	group_threshold �C Coefficient to regulate the similarity threshold. When detected, some objects can be covered by many rectangles. 
					  0 means not to perform grouping. See groupRectangles() .
					
*/


// draw the output boxes with 'red' color
void Class_HoG_LinearSVM::myShowDets(Mat &im, const vector<ObjectDetection> &dets)
{
	Scalar color(0, 0, 255);	// red color
	int num = dets.size();
	char tmp[10];
	for(int i=0; i<num; ++i)
	{
		Rect locationRect = dets[i].rect;
		sprintf(tmp, "%.2f", dets[i].score);
		string scoreText = (string)tmp;
		rectangle(im, locationRect, color, 2);
		putText(im, scoreText, Point(locationRect.x+5, locationRect.y+15), FONT_HERSHEY_SIMPLEX, 0.55, color, 2);
	}
}