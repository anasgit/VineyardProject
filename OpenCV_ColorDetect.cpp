//OpenCV ColorDetect
//hiranyajaya - May 2016
//Source from http://opencv-srf.blogspot.com/

#include <Windows.h>
#include <iostream>
#include <conio.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;
using namespace std;



//Trackbar callback function
void MyCallbackForBrightness(int brightnessVal, void *userData);
void MyCallbackForContrast(int contrastVal, void *userData);

//Mouse event callback function
void CallBackFunc(int event, int x, int y, int flags, void* userdata);

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
	Mat img01HSV;
	Mat img01Thres;
	
	// upper and lower bounds of Hue, sauration and value.
	int lowHue = 0;
	int highHue = 179;
	int lowSat = 0; 
	int highSat = 255;
	int lowVal = 0;
	int highVal = 255;

	int esc; //esc key

	// Create Window
	namedWindow("Display", WINDOW_AUTOSIZE); //CV function- create window
	namedWindow("Control Panel", WINDOW_AUTOSIZE);

	//Create trackbars for "Control Panel" window ( in while(1) method)
	cvCreateTrackbar("LowH", "Control Panel", &lowHue, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control Panel", &highHue, 179);

	cvCreateTrackbar("LowS", "Control Panel", &lowSat, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control Panel", &highSat, 255);

	cvCreateTrackbar("LowV", "Control Panel", &lowVal, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control Panel", &highVal, 255);

	//Create or Read Image
	img01 = imread("balls.JPG",CV_LOAD_IMAGE_UNCHANGED); //load from file
	
	// Check whether it is read properly.
	if (img01.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		//system("pause"); //wait for a key press
		getch();
		return -1;
	}

	cvtColor(img01, img01HSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	
	
	

	

	while(1)
	{
	
    // Select pixels in a specific HSV range
	inRange(img01HSV, Scalar(lowHue, lowSat, lowVal), Scalar(highHue, highSat, highVal), img01Thres);

	/* More on inRange() function
	void inRange(InputArray src, InputArray lowerb, InputArray upperb, OutputArray dst);
	InputArray src - Source image
	InputArray lowerb - Inclusive lower boundary (If lowerb=Scalar(x, y, z), pixels which have values lower than x, y and z for HUE, SATURATION and VALUE respectively is considered as black pixels in dst image)
	InputArray upperb -  Exclusive upper boundary (If it is upperb=Scalar(x, y, z), pixels which have values greater or equal than x, y and z for HUE, SATURATION and VALUE respectively is considered as black pixels in dst image)
	OutputArray dst -  Destination image (should have the same size as the src image and should be 8-bit unsigned integer, CV_8U)
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
		break;

	}
	waitKey(0); //CV function

	destroyWindow("Display"); // CV function
	//destroyAllWindows();
	
	return 0;

}
