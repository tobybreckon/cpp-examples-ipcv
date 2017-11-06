// Example : Bilateral Filtering of image / video / camera
// usage: prog {<image_name> | <video_name>}

// Author : Toby Breckon, toby.breckon@durham.ac.uk

// Copyright (c) 2012 School of Engineering, Cranfield University
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
// setup the cameras properly based on OS platform

// 0 in linux gives first camera for v4l
//-1 in windows gives first device or user dialog selection

#ifdef linux
	#define CAMERA_INDEX 0
#else
	#define CAMERA_INDEX -1
#endif

/******************************************************************************/

int main( int argc, char** argv )
{
  Mat img, res;	    // image objects
  VideoCapture cap; // capture object

  const string windowName = "Input"; // window name
  const string windowName2 = "Bilateral Filtered Output"; // window name

  bool keepProcessing = true;	// loop control flag
  char  key;						// user input
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

  int d = 5;			    // Bilateral filtering parameters
  int sigmaS = 50;
  int sigmaR = 50;

  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera

    if(
	  ( argc == 2 && (!(img = imread( argv[1], CV_LOAD_IMAGE_COLOR)).empty()))||
	  ( argc == 2 && (cap.open(argv[1]) == true )) ||
	  ( argc != 2 && (cap.open(CAMERA_INDEX) == true))
	  )
    {
      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      namedWindow(windowName, 0);
      namedWindow(windowName2, 0);

        createTrackbar( "d - pixel neighbourhood", windowName2, &d, 25);
        createTrackbar( "sigma S", windowName2, &sigmaS, 250);
        createTrackbar( "sigma R", windowName2, &sigmaR, 250);

	  // start main loop

	  while (keepProcessing) {

          int64 timeStart = getTickCount(); // get time at start of loop

		  // if capture object in use (i.e. video/camera)
		  // get image from capture object

		  if (cap.isOpened()) {

			  cap >> img;
			  if(img.empty()){
				if (argc == 2){
					std::cerr << "End of video file reached" << std::endl;
				} else {
					std::cerr << "ERROR: cannot get next fram from camera"
						      << std::endl;
				}
				exit(0);
			  }

		  }	else {

			  // if not a capture object set event delay to zero so it waits
			  // indefinitely (as single image file, no need to loop)

			  EVENT_LOOP_DELAY = 0;
		  }

		  // ***

		  // d – Diameter of each pixel neighborhood that is used during filtering.
		  // If it is non-positive, it is computed from sigmaSpace .

          // sigmaR – Filter sigma in the color space. A larger value of the parameter means
          // that farther colors within the pixel neighborhood (see sigmaSpace ) will be mixed
          // together, resulting in larger areas of semi-equal color.

          // sigmaS – Filter sigma in the coordinate space. A larger value of the parameter
          // means that farther pixels will influence each other as long as their colors are
          // close enough (see sigmaColor ). When d>0 , it specifies the neighborhood
          // size regardless of sigmaSpace . Otherwise, d is proportional to sigmaSpace .

          bilateralFilter(img, res, d, (double) sigmaR, (double) sigmaS, BORDER_DEFAULT );

		  // ***

		  // display image in window

      imshow(windowName, img);
		  imshow(windowName2, res);

          // start event processing loop (very important,in fact essential for GUI)
          // 40 ms roughly equates to 1000ms/25fps = 4ms per frame

          // here we take account of processing time for the loop by subtracting the time
          // taken in ms. from this (1000ms/25fps = 40ms per frame) value whilst ensuring
          // we get a +ve wait time

		  key = waitKey((int) std::max(2.0, EVENT_LOOP_DELAY -
                        (((getTickCount() - timeStart) / getTickFrequency()) * 1000)));

		  if (key == 'x'){

	   		// if user presses "x" then exit

			  	std::cout << "Keyboard exit requested : exiting now - bye!"
				  		  << std::endl;
	   			keepProcessing = false;
		  }
	  }

	  // the camera will be deinitialized automatically in VideoCapture destructor

      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
/******************************************************************************/
