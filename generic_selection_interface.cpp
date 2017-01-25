// Example : generic interface to image / video / camera
// usage: prog {<image_name> | <video_name>}

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
// setup the cameras properly based on OS platform

// 0 in linux gives first camera for v4l
//-1 in windows gives first device or user dialog selection

#ifdef linux
	#define CAMERA_INDEX  1
#else
	#define CAMERA_INDEX -1
#endif

/******************************************************************************/

// callback funtion for mouse to select a region of the image and store that selection
// in global variables origin and selection (acknowledgement: opencv camsiftdemo.cpp)

static bool selectObject = false;
static Point origin;
static Rect selection;
static bool selectionComplete = false;

void onMouseSelect( int event, int x, int y, int, void* image)
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= Rect(0, 0, ((Mat *) image)->cols, ((Mat *) image)->rows);
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case CV_EVENT_LBUTTONUP:
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 )
            selectionComplete = true;
        break;
    }
}

/******************************************************************************/

int main( int argc, char** argv )
{

  Mat img, roi, selected;			// image object
  VideoCapture cap; // capture object

  const string windowName = "Live Video Input"; // window name
  const string windowName2 = "Selected Region / Object"; // window name

  bool keepProcessing = true;	// loop control flag
  unsigned char key;						// user input
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera

    if(
	  ( argc == 2 && (cap.open(argv[1]) == true )) ||
	  ( argc != 2 && (cap.open(CAMERA_INDEX) == true))
	  )
    {
      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      namedWindow(windowName, 0);
      namedWindow(windowName2, 0);
      setMouseCallback( windowName, onMouseSelect, &img);

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
					std::cerr << "ERROR: cannot get next frame from camera"
						      << std::endl;
				}
				exit(0);
			  }

		  }

		  // ***

		  // *** DO ANY PROCESSING PRIOR TO DISPLAY HERE ***

		  // ***


          if( selectObject && selection.width > 0 && selection.height > 0 )
          {
            roi = img(selection);
            bitwise_not(roi, roi);
          } else if ( selectionComplete && selection.width > 0 && selection.height > 0 ){

            selected = roi.clone();
            selectionComplete = false;

          }

		  // display image in window

		  imshow(windowName, img);
		  if (!(selected.empty()))
		  {
                imshow(windowName2, selected);
		  }

		  // start event processing loop (very important,in fact essential for GUI)
	      // 40 ms roughly equates to 1000ms/25fps = 4ms per frame

          // here we take account of processing time for the loop by subtracting the time
          // taken in ms. from this (1000ms/25fps = 40ms per frame) value whilst ensuring
          // we get a +ve wieght time

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
