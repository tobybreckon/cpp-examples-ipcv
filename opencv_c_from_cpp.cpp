// Example : example of using OpenCV C code from OpenCV C++ code
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
	#define CAMERA_INDEX 0
#else
	#define CAMERA_INDEX -1
#endif

/******************************************************************************/

// !!!!!!! EXAMPLE OpenCV C function from c/contraststretch.cc example !!!!!!!!!

// function that takes a gray scale image and draws a histogram
// image for it in a pre-allocated image

void create_histogram_image(IplImage* grayImg, IplImage* histogramImage){

  CvHistogram *hist = NULL;	    // pointer to histogram object
  float max_value = 0;			// max value in histogram
  int hist_size = 256;			// size of histogram (number of bins)
  int bin_w = 0;				// initial width to draw bars
  float range_0[]={0,256};
  float* ranges[] = { range_0 };

  hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

  cvCalcHist( &grayImg, hist, 0, NULL );
  cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
  cvScale( hist->bins, hist->bins, ((double)histogramImage->height)/max_value, 0 );
  cvSet( histogramImage, cvScalarAll(255), 0 );
  bin_w = cvRound((double)histogramImage->width/hist_size);

  for(int i = 0; i < hist_size; i++ )
  {
     cvRectangle( histogramImage, cvPoint(i*bin_w, histogramImage->height),
                  cvPoint((i+1)*bin_w, histogramImage->height
	  								- cvRound(cvGetReal1D(hist->bins,i))),
                   					cvScalarAll(0), -1, 8, 0 );
  }

  cvReleaseHist (&hist);
}

/******************************************************************************/

int main( int argc, char** argv )
{

  Mat img;			// image object
  Mat gray;			// gray image object
  Mat histogram;	// histogram image object
  VideoCapture cap; // capture object

  const string windowName = "Input Image (OpenCV C++ code)"; // window name
  const string windowName2 = "Histogram (OpenCV C code)"; // window name
  const string windowName3 = "Canny (OpenCV C/C++ objects)"; // window name

  bool keepProcessing = true;	// loop control flag
  unsigned char key;						// user input
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

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
	  namedWindow(windowName2,0);
	  namedWindow(windowName3,0);

	  // create trackbars for the canny stuff

	  int upper = 200;
	  int lower = 80;
	  createTrackbar((string) "upper", windowName3, &upper, 255, 0, 0);
	  createTrackbar((string) "lower", windowName3, &lower, 255, 0, 0);

	  // start main loop

	  while (keepProcessing) {

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

		  // convert input image to grayscale

		  cvtColor(img, gray, CV_BGR2GRAY);

		  // here we must explicitly specify the size of the histogram image
		  // as it is expected to be pre-allocated by our OpenCV C function
		  // (255 = columns for our histogram)

		  histogram = Mat(200, 256,  CV_8UC1);

		  // to use OpenCV C code with C++ OpenCV image objects we can use the
		  // Mat::IplImage() operator to return an (old-style) IplImage header
		  // for the Mat object/image data (N.B. this is a pointer to the
		  // original Mat object and no data is copied). Where an IplImage*
		  // (pointer) is required we use &() to return this temporary pointer

		  // here we do this for both input and outputs

			IplImage *gray_ipl_p = &(IplImage(gray));

		  create_histogram_image(gray_ipl_p, &(IplImage(histogram)));

		  // to use (old-style) C OpenCV image objects with C++ OpenCV methods
		  // we use the following inverse operator

		  IplImage *test = gray_ipl_p; // create an old style C IplImage object
		  IplImage *canny = 				  // and also an output IplImage image
			  cvCreateImage(cvSize(test->width,test->height), test->depth, 1);

		  // use both of these OpenCV C images with an OpenCV C++ function

		  Mat tmp_test = Mat(cv::cvarrToMat(test));
		  Mat tmp_canny = Mat(cv::cvarrToMat(canny));
		  Canny(tmp_test, tmp_canny, lower, upper, 3);

		  // display all images in window

		  imshow(windowName, gray);
		  imshow(windowName2, histogram);
		  imshow(windowName3, Mat(cv::cvarrToMat(canny)));

		  // start event processing loop (very important,in fact essential for GUI)
	      // 40 ms roughly equates to 1000ms/25fps = 4ms per frame

		  key = waitKey(EVENT_LOOP_DELAY);

		  if (key == 'x'){

	   		// if user presses "x" then exit

			  	std::cout << "Keyboard exit requested : exiting now - bye!"
				  		  << std::endl;
	   			keepProcessing = false;
		  }

	  // explicitly created OpenCV C images/matrices must be deallocated

	  cvReleaseImage(&canny);

	  }

	  // the camera will be deinitialized automatically in VideoCapture destructor

      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
/******************************************************************************/
