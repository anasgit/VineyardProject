//OpenCV Object Detect and contours - Part 02
//hiranyajaya - May 2016
//Source from OpenCV documentation

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

	Mat img01;
	Mat img01B, img01GRAY, img01HSV, img01HSVinRange, img01Thres, img01Canny, img01Dist;
	Mat channels[3];
	Mat img01chV;
	Vec3b color;

	int bl = 1;
	int thresLevel = 125;
	int esc;
	
	// upper and lower bounds of Hue, sauration and value.
	int lowHue = 0;
	int highHue = 179;
	int lowSat = 0; 
	int highSat = 255;
	int lowVal = 0;
	int highVal = 255;

	// Create Window
	namedWindow("Display", WINDOW_AUTOSIZE); //CV function- create window
	namedWindow("Blur and Threshold", WINDOW_AUTOSIZE);
	namedWindow("Control Panel", WINDOW_AUTOSIZE);
	
	//Create or Read Image
	img01 = imread("grapes.JPG",CV_LOAD_IMAGE_COLOR); //load from file

	// Check whether it is read properly.
	if (img01.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		//system("pause"); //wait for a key press
		getch();
		return -1;
	}

	//Create Trackbars
	createTrackbar("blur amount","Blur and Threshold",&bl,20);
	createTrackbar( "Threshold", "Blur and Threshold", &thresLevel, 255);

	//Create trackbars for "Control Panel" window ( in while(1) method)
	cvCreateTrackbar("LowH", "Control Panel", &lowHue, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control Panel", &highHue, 179);

	cvCreateTrackbar("LowS", "Control Panel", &lowSat, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control Panel", &highSat, 255);

	cvCreateTrackbar("LowV", "Control Panel", &lowVal, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control Panel", &highVal, 255);

	
	
	for( int x = 0; x < img01.cols; x++ ) {
      for( int y = 0; y < img01.rows; y++ ) {
		  color = img01.at<Vec3b>(Point(x,y));
          if ( (color[0] > 230) && (color[1] > 230) && (color[2] > 230)) {
            color[0] = 0;
            color[1]= 0;
            color[2] = 0;
			
		  img01.at<Vec3b>(Point(x,y))= color;
          }
        }
    }
		
	
	while(1)
	{
	
	   blur( img01, img01B, Size(bl,bl)); // blur/smooth function
	   //medianBlur( img01, img01B, bl );
	   //bilateralFilter( img01, img01B, bl, bl, bl);
	   //GaussianBlur( img01, img01B, Size( bl, bl ), 0, 0 );

	   cvtColor(img01B, img01GRAY,COLOR_BGR2GRAY); //Convert the captured frame from BGR to GRAYSCALE

	   //cvtColor(img01B, img01HSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	   //split(img01HSV, channels);
	   //img01chV = channels[2];

	   //inRange(img01HSV, Scalar(lowHue, lowSat, lowVal), Scalar(highHue, highSat, highVal), img01HSVinRange);
	   threshold(img01GRAY,img01Thres,thresLevel,255,CV_THRESH_BINARY);  // Mainly used in greyscale image threshold

	   // Learn more on morphological operations
	   //morphological opening (remove small objects from the foreground)
	   erode(img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	   dilate( img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

	   //morphological closing (fill small holes in the foreground)
	   dilate( img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
	   erode(img01Thres, img01Thres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );


	   // Canny detects edges
	   //Canny( img01Thres, img01Canny, 50, 150, 3);

	   // Apply distance transform
	   distanceTransform(img01Thres, img01Dist, CV_DIST_L2, 3);
	   normalize(img01Dist, img01Dist, 0, 1., NORM_MINMAX);


	   imshow("Display", img01Dist); //CV function-show image
	   
	   esc = waitKey(2000);

	  if (esc==27)
		{
			cout << "Proceeding to the second part" << endl;
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
	approxPolyDP( Mat(contours[i]), contours_poly[i], 1, true ); // Approximate the contour to a polygon based on the epsilon value provided.
	boundRect[i] = boundingRect( Mat(contours_poly[i])); // returns properties for a bounding rectangle ( no rotation considered)
	minEnclosingCircle( contours_poly[i], center[i], radius[i] );
	convexHull(Mat(contours[i]), contours_convexHull[i]);
	}

	// Mat to draw the contours
	Mat drawing = Mat::zeros( img01Thres.size(), CV_8UC3 );

	for( size_t i = 0; i< contours.size(); i++ )
	{
	
		
		Scalar color = Scalar( 130, 55, 28 );
		drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() ); // draw contour based on polygon approximation which returns contours_poly
		drawContours( drawing, contours_convexHull, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() ); // draw contour based on convex hull results based on contours_convexHull
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
		circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
		//useful contour features follow.
		cout << "Area of contour :" << contourArea(contours[i]) <<endl; // Area 
		cout << "Number of vertices  :" << contours_poly[i].size() << endl; // vertices

		//Extracting features through moments
		Moments M = moments(contours[i]);
		double Area = M.m00;
		double m01 = M.m01;
		double m10 = M.m10;

		cout << "centroid of the contour : Coord X :" << (int)(m10/Area) << " Coord Y : " << (int)(m01/Area) << " Area : " << Area << endl; 
		cout << "Arc length of the contour :" << arcLength(contours[i],TRUE) << endl; // length of the contour

		// Convex hull - assumes the image should not have 'dents' and fixes those faults.
		cout << "Convexity Check :" << isContourConvex(contours[i]) << endl;

		
	}

	cout << " Number of objects detected :" << contours.size() << endl;

	imshow("Display", drawing); //CV function-show image

	waitKey(0); //CV function
	
	
	
	waitKey(0);
	return 0;


}