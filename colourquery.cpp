// Example : query colour elements in an image
// usage: prog <image_name>

// Author : Toby Breckon, toby.breckon@durham.ac.uk

// Copyright (c) 2010 School of Engineering, Cranfield University
// Copyright (c) 2016 School of Engineering & Computing Sciences, Durham University
// License : LGPL - http://www.gnu.org/licenses/lgpl.html

#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>		// standard C++ I/O
#include <string>		// standard C++ I/O
#include <algorithm>    // includes max()

using namespace cv; // OpenCV API is in the C++ "cv" namespace
using namespace std;

/******************************************************************************/

void colourQueryMouseCallBack(int event, int x, int y, int flags, void* img)
{

	int row = y; // y-axis is image rows (down the side)
	int col = x; // x-axis is image columns (along the top)

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN :

		// left button prints colour information at click location to stdout

		std::cout << "Colour information at image location (" << x << ","
			<< y << "): ";

		for(int channel = 0; channel < 3; channel++){ // three channels (B,G,R)

			// note that variable img is now a pointer to the image object
			// in this case (as it was passed by reference)

			// IN GENERAL: pixel access is img.at<Vec3b>(row,col)[channel] for
			// a 3 channel image (3 bytes per pixel) and img.at<uchar>(row,col) for
			// a single channel image (1 byte per pixel)

			std::cout <<
			(unsigned int) ((Mat*) img)->at<Vec3b>(row,col)[channel] << " ";
		}

		std::cout << std::endl;

		;
		break;
	case CV_EVENT_RBUTTONDOWN :

		// right button sets colour information at click location to white

		std::cout << "Colour information at image location (" << x << "," << y
				<< ") set to white.";

		for(int channel = 0; channel < 3; channel++){ // three channels (B,G,R)
			((Mat*) img)->at<Vec3b>(row,col)[channel] = 255;
		}
		std::cout << std::endl;

		;
		break;
	}
}

/******************************************************************************/

int main( int argc, char** argv )
{

  Mat img;  // image object
  unsigned char key;
  bool keepProcessing = true;

  const string windowName = "OPENCV: colour query"; // window name

  // check that command line arguments are provided and image reads in OK

  if ((argc == 2) && !(img = imread( argv[1], CV_LOAD_IMAGE_COLOR)).empty())
  {
	// create window object

	namedWindow(windowName, 0 );

	// set function to be executed everytime the mouse is clicked/moved
	// (note: this uses the older cvXXX function naming style from the
	// OpenCV C interface)

	cvSetMouseCallback("OPENCV: colour query",
			(CvMouseCallback) colourQueryMouseCallBack, &img);


	// print out some helpful information about the image

	std::cout << "Image : (width x height) = (" << img.cols << " x "
			<< img.rows << ")" << std::endl;
	std::cout << "         Colour channels = " << img.channels() << std::endl;

	// loop so that events are processed and the image constantly redisplayed

	  while (keepProcessing){

		// display image in window

		imshow(windowName, img );

      		// start event processing loop (very important,in fact essential for GUI)

      		key=waitKey(20);

		// get any keyboard input given by the user and process it

		if (key == 'x'){

			// if user presses "x" then exit

			std::cout << "Keyboard exit requested : exiting now - bye!" << std::endl;
			keepProcessing = false;
		}

	}

      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}

/******************************************************************************/
