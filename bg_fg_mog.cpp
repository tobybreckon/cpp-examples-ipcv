// Example : background / foreground separation of video / camera
// usage: prog {<video_name>}

// Author : Toby Breckon, toby.breckon@cranfield.ac.uk

// Author : Toby Breckon, toby.breckon@durham.ac.uk

// Copyright (c) 2012 School of Engineering, Cranfield University
// Copyright (c) 2016 School of Engineering & Computing Sciences, Durham University
// License : LGPL - http://www.gnu.org/licenses/lgpl.html

// #include <cv.h>   		// open cv general include file
// #include <highgui.h>	// open cv GUI include file

// #include <opencv2/video/background_segm.hpp> // OpenCV BG/FG specific header

// #include <iostream>		// standard C++ I/O
// #include <algorithm>    // includes max()

// using namespace cv; // OpenCV API is in the C++ "cv" namespace

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include <iostream>


using namespace cv;
using namespace std;

/******************************************************************************/

int main( int argc, char** argv )
{

  Mat img, fg, fg_msk, bg;	// image objects
  VideoCapture cap;     // capture object

  const string windowName = "Live Image"; // window name
  const string windowNameF = "Foreground"; // window name
  const string windowNameB = "Background"; // window name

  bool keepProcessing = true;	// loop control flag
  unsigned char  key;			// user input
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera

    if(( argc == 2 && (cap.open(argv[1]) == true )) ||
	  ( argc != 2 && (cap.open(0) == true)))
    {
      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      namedWindow(windowName, 0);
      namedWindow(windowNameF, 0);
      namedWindow(windowNameB, 0);

      // create background / foreground Mixture of Gaussian (MoG) model

      Ptr<BackgroundSubtractorMOG2> MoG = createBackgroundSubtractorMOG2();

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

		  // update background model and get background/foreground

		  MoG->apply(img, fg_msk, 0.001);
		  MoG->getBackgroundImage(bg);

          fg = Scalar::all(0);
          img.copyTo(fg, fg_msk);

		  // display image in window

		  imshow(windowName, img);
          imshow(windowNameF, fg);
          if (!bg.empty())
          {
            imshow(windowNameB, bg);
          }

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
