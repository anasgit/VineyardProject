//OpenCV Object Detect and contours
//hiranyajaya - May 2016
//Source from http://opencv-srf.blogspot.com/

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

// Struct to retrive data from callback functions if needed.
struct imgUpdate
{
    Point p;
    int contrast;
    int brightness;
};

int main()
{
	/* Color Detection :
	HSV - Hue-Saturation-Value are better for color detection than the RGB space
	Hue - Color(0-179) : Orange  0-22; Yellow 22- 38; Green 38-75; Blue 75-130; Violet 130-160; Red 160-179
	Saturation - hoe much the color is mixed with white
	Value - how much the color is mixed with black
	*/

	Mat img01; // main image used across several functions
	Mat img01B;
	Mat img01HSV;
	Mat img01GRAY;
	Mat img01HSVinRange;
	Mat img01Thres;
	
	//Grayscale threshold
	int grayThres = 170;

	// upper and lower bounds of Hue, sauration and value.
	int lowHue = 0;
	int highHue = 179;
	int lowSat = 0; 
	int highSat = 255;
	int lowVal = 0;
	int highVal = 255;


	int esc; //esc key
	int i=1; //blur amount
	
	// Create Window
	namedWindow("Display", WINDOW_AUTOSIZE); //CV function- create window
	namedWindow("Blur and Threshold", WINDOW_AUTOSIZE);
	namedWindow("Control Panel", WINDOW_AUTOSIZE);

	//Create or Read Image
	img01 = imread("Shapes.JPG",CV_LOAD_IMAGE_UNCHANGED); //load from file

	// Check whether it is read properly.
	if (img01.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		//system("pause"); //wait for a key press
		getch();
		return -1;
	}
	

	createTrackbar("blur amount","Blur and Threshold",&i,20);

	// Threshold trackbar to divide image to black and white
	createTrackbar( "Threshold", "Blur and Threshold", &grayThres, 255);

	//Create trackbars for "Control Panel" window ( in while(1) method)
	cvCreateTrackbar("LowH", "Control Panel", &lowHue, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control Panel", &highHue, 179);

	cvCreateTrackbar("LowS", "Control Panel", &lowSat, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control Panel", &highSat, 255);

	cvCreateTrackbar("LowV", "Control Panel", &lowVal, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control Panel", &highVal, 255);


	// Apply trackbar effect to the image
	while(1)
	{

    bilateralFilter( img01, img01B, i, i, i);
	//medianBlur( img01, img01B, i );

	//cvtColor(img01, img01GRAY,COLOR_BGR2GRAY); //Convert the captured frame from BGR to GRAYSCALE
	cvtColor(img01B, img01HSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	// Select pixels in a specific HSV range
	inRange(img01HSV, Scalar(lowHue, lowSat, lowVal), Scalar(highHue, highSat, highVal), img01HSVinRange);
	threshold(img01HSVinRange,img01Thres,grayThres,255,CV_THRESH_BINARY);  // Mainly used in greyscale image threshold

	/* More on threshold() function
	const Mat& src - Source array (This should be single channel)
	Mat& dst - Destination array which has the same size and same type as the 'src'
	double threshVal - Threshold value
	double max - Maximum value to use with 'THRESH_BINARY' and 'THRESH_BINARY_INV' which are thresholding types
	int thresholdType - You can use one of the following for this arguement
	THRESH_BINARY
	dst(x,y)=max,             if src(x,y) > ThreshVal
	dst(x,y)=0,                 if src(x,y) < ThreshVal
	THRESH_BINARY_INV
	dst(x,y)=0,                  if src(x,y) > ThreshVal
	dst(x,y)=max,              if src(x,y) < ThreshVal
	THRESH_TOZERO
	dst(x,y)=src(x,y),       if src(x,y) > ThreshVal
	dst(x,y)=0,                 if src(x,y) < ThreshVal
	THRESH_TOZERO_INV
	dst(x,y)=0,                  if src(x,y) > ThreshVal
	dst(x,y)=src(x,y),        if src(x,y) < ThreshVal
	THRESH_TRUNC
	dst(x,y)=threshVal,    if src(x,y) > ThreshVal
	dst(x,y)=src(x,y),        if src(x,y) < ThreshVal
	*/

	// Learn more on morphological operations
	//morphological opening (remove small objects from the foreground)
	erode(img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate( img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

	//morphological closing (fill small holes in the foreground)
	dilate( img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
	erode(img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    
	//Disply image in Window
	imshow("Display", img01Thres); //CV function-show image
	
	esc = waitKey(20);

	if (esc==27)
	{
		cout << " Current HSV and threshold values applied to the image. " << endl;
		break;
	}

	}

	// variables for contours
	vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
	//Mat hierarchy;

	// find contours for in the image and save data in the contours vector.
	findContours( img01Thres, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

	

	//vector variables for polygon drawing. VAriables need to be defined after the size of countour vector is found.
	vector<vector<Point> > contours_poly( contours.size() );
	vector<vector<Point>> contours_convexHull ( contours.size());
	vector<Rect> boundRect( contours.size() );
	vector<RotatedRect> rotRect ( contours.size());
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );
	 
	for( size_t i = 0; i < contours.size(); i++ )
	{ 
	approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true ); // Approximate the contour to a polygon based on the epsilon value provided.
	boundRect[i] = boundingRect( Mat(contours_poly[i])); // returns properties for a bounding rectangle ( no rotation considered)
	minEnclosingCircle( contours_poly[i], center[i], radius[i] );
	convexHull(Mat(contours[i]), contours_convexHull[i]);
	rotRect[i] = fitEllipse(Mat(contours[i])); // Doesn't work check
	}

	// Mat to draw the contours
	Mat drawing = Mat::zeros( img01Thres.size(), CV_8UC3 );

	for( size_t i = 0; i< contours.size(); i++ )
	{
		if (contours_poly[i].size() == 3)
		{
		Scalar color = Scalar( 130, 55, 28 );
		drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() ); // draw contour based on polygon approximation which returns contours_poly
		drawContours( drawing, contours_convexHull, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() ); // draw contour based on convex hull results based on contours_convexHull
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
		circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
		ellipse(drawing,rotRect[i],( 100,100,100),3); // doesn't work Check.
		//useful contour features follow.
		cout << "Area of the triangle :" << contourArea(contours[i]) <<endl; // Area 
		cout << "Number of vertices in the triangle ( duh !) :" << contours_poly[i].size() << endl; // vertices

		//Extracting features through moments
		Moments M = moments(contours[i]);
		double Area = M.m00;
		double m01 = M.m01;
		double m10 = M.m10;

		cout << "centroid of the triangle : Coord X :" << (int)(m10/Area) << " Coord Y : " << (int)(m01/Area) << " Area : " << Area << endl; 
		cout << "Arc length of the triangle :" << arcLength(contours[i],TRUE) << endl; // length of the contour

		// Convex hull - assumes the image should not have 'dents' and fixes those faults.
		cout << "Convexity Check :" << isContourConvex(contours[i]) << endl;

		}
	}

	imshow("Display", drawing); //CV function-show image

	waitKey(0); //CV function

	destroyWindow("Display"); // CV function
	//destroyAllWindows();
	
	return 0;

}
