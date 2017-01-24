// Example : grab and display a single live image
// usage: prog

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

int main( int argc, char** argv )
{

  Mat img;  // image object

  const string windowName = "OPENCV: live image display"; // window name

  // create window object

  namedWindow(windowName, 1);

  // grab an image from camera (here assume only 1 camera, device 0)

  VideoCapture cap(0);  // video capture object

  if(!cap.isOpened()){
    std::cout << "error: could not grab a frame" << std::endl;
    exit(0);
  }

  cap >> img; // retrieve the captured frame as an image

  // display image in window

  imshow(windowName, img);

  // start event processing loop (very important,in fact essential for GUI)

  cvWaitKey(0);

  // the camera will be deinitialized automatically in VideoCapture destructor

  // all OK : main returns 0

  return 0;

}
