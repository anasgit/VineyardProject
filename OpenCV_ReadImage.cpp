
//#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

int main()
{

	Mat img01 = imread("Auris.JPG",CV_LOAD_IMAGE_UNCHANGED);

	if (img01.empty())
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
        //system("pause"); //wait for a key press
		getch();
        return -1;
	}

	namedWindow("Image Display", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
    imshow("Image Display", img01); //display the image which is stored in the 'img' in the "MyWindow" window

	
	waitKey(0); //CV function

	destroyWindow("Image Display"); // CV function
	 	
	return 0;
}