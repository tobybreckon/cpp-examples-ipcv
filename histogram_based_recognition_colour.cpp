// Example : RGB colour histogram based recognition from video / camera
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

void printhelp(){
	std::cout << std::endl << "Controls:" << std::endl;
	std::cout << "\tspace = capture a sample image" << std::endl;
	std::cout << "\treturn = move to recognition mode (or m)" << std::endl;
	std::cout << "\tr = recognise current image" << std::endl;
	std::cout << "\tany key = clear recognition result" << std::endl;
	std::cout << "\tx = exit" << std::endl;
}

/******************************************************************************/

int main( int argc, char** argv )
{

  Mat img;					// image objects
  VideoCapture cap; 		// capture object

  const string windowName = "Colour Histogram Based Recognition"; // window name

  bool keepProcessing = true;	// loop control flag
  int key;						// user input
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

  // histogram specific stuff - create a 3D histogram for RGB image

  #define MAX_NUMBER_OF_SAMPLE_IMAGES 255
  int hist_size[] = {256, 256, 256};	// size of histogram (number of bins)
  float range_0[]={0, 255};
  const float* ranges[] = { range_0, range_0, range_0 };
  int channels[] = {0, 1, 2};   // we compute the histogram from all 3 channels

  // create the histogram explicitlty, specifying 32-bit float (CV_32F) data
  // storage

  MatND currentHistogram;
  currentHistogram.create(3, hist_size, CV_32F);

  // data structures and matrices for histogram based recognition

  Mat input[MAX_NUMBER_OF_SAMPLE_IMAGES];
  MatND histogram[MAX_NUMBER_OF_SAMPLE_IMAGES];

  int imagesCollected = 0;			// number of sample images collected

  bool recognitionStage = false;	// flag to determine when have started
  									// recognition

  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera

  if(
	  ( argc == 2 && (cap.open(argv[1]) == true )) ||
	  ( argc != 2 && (cap.open(CAMERA_INDEX) == true))
	  )
    {
	  // print user controls

	  printhelp();

      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      namedWindow(windowName, 0);

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

		  }

	  	  // flip the image (so movement on screen matches movement in window)

		  flip(img, img, 1);

		  // display image in window (with text)

	      if (!recognitionStage){
			putText(img, "SAMPLE COLLECTION", Point(10,img.rows - 10),
					  FONT_HERSHEY_PLAIN, 2.0, CV_RGB(0, 255,0), 3, 8, false);
		  } else {
			putText(img, "RECOGNITION", Point(10,img.rows - 10),
					  FONT_HERSHEY_PLAIN, 2.0, CV_RGB(255, 0,0), 3, 8, false);
		  }
		  imshow( windowName, img );

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

		  }  else if (key == ' '){

	   		// if user presses " " then capture a sample image

			if (!recognitionStage) {
				 if (imagesCollected < MAX_NUMBER_OF_SAMPLE_IMAGES)
				 {

					// copy image + build/store image histogram

				    input[imagesCollected] = img.clone();

					calcHist(&img, 1, channels, Mat(), currentHistogram, 3,
							 hist_size, ranges, true, false);
					normalize(currentHistogram, currentHistogram, 1, 0, NORM_L1);
					histogram[imagesCollected] =  currentHistogram.clone();

					imagesCollected++;

					 std::cout << "Sample image collected - " <<
						 imagesCollected << std::endl;

				 } else {
					std::cout <<  "ERROR: Maximum sample images (" <<
						 imagesCollected << ") collected" << std::endl;
				 }
		   }

		  } else if ((key == '\n') || (key == 'm')) { // use "m" in windows

	   		// if user presses return then move into recognition mode

		    std::cout << "Entering recognition mode - histogram models" <<
				 " stored" << std::endl << std::endl;

		    recognitionStage = true;
		    if (!(imagesCollected > 0)) {
			    std::cerr << "ERROR: not enough samples images caputured" <<
					std::endl;
		    }
		  } else if (key == 'r'){

			// if user presses "r" then do recognition

			  // calc current image histogram

			  calcHist(&img, 1, channels, Mat(), currentHistogram, 3,
						hist_size, ranges, true, false);
			  normalize(currentHistogram, currentHistogram, 1, 0, NORM_L1);

			  if (recognitionStage) {

				// for each histogram (do comparison)

				double closestDistance = __DBL_MAX__;
				int closestImage = 0;

				for (int i = 0; i < imagesCollected; i++)
				{
					// do histogram comparision here

					double correlation = compareHist(currentHistogram,
												histogram[i],HISTCMP_CORREL);
					double chisquared = compareHist(currentHistogram,
												histogram[i],HISTCMP_CHISQR);
					double intersect = compareHist(currentHistogram,
												histogram[i],HISTCMP_INTERSECT);
					double bhattacharyya = compareHist(currentHistogram,
												histogram[i],HISTCMP_BHATTACHARYYA);

					// here we just sum the differences of the measures
					// (which as the histograms are all normalised are all
					// measures in the range -1->0->1). This *is not* the
					// best way to do this - beware.

					// N.B. For the OpenCV implementation:
					// low correlation = large difference (so we invert it)
					// low intersection = large difference (so we invert it)
					// high chisquared = large differences
					// high bhatt. = large difference
					// - and vice versa

					double diff = (1 - correlation) + chisquared
											+ (1 - intersect) + bhattacharyya;

					std::cout <<  "Comparison image " << i << " Corr: " <<
						correlation << " ChiSq: " << chisquared <<
						" Intersect: " << intersect << " Bhatt: " << bhattacharyya
						<< " Total Distance = " << diff << std::endl;

					if (diff < closestDistance){
						closestDistance = diff;
						closestImage = i;

					}
				}

				std::cout <<  std::endl;

				// output the result in a window

				std::cout << "Recognition - closest matching image = " <<
					 closestImage << std::endl;
				std::cout <<  "Press any key to clear." << std::endl << std::endl;

				namedWindow("Recognition Result", 1 );
                imshow("Recognition Result", input[closestImage]);
				waitKey(0);
				destroyWindow("Recognition Result"); // close window

			} else {
				std::cout << "ERROR - need to enter recognition stage first."
					<< std::endl;
			}
		  }
	  }


	  // all image and histogram objects should be killed off by their respective
	  // destructors

	  // the camera will be deinitialized automatically in VideoCapture destructor

      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
/******************************************************************************/
