// Example : basic interface to example based recognition from video / camera
// usage: prog {<video_name>}

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

void printhelp()
{
    std::cout << std::endl << "Controls:" << std::endl;
    std::cout << "\tspace = capture a sample image" << std::endl;
    std::cout << "\treturn = move to recognition mode (or m)" << std::endl;
    std::cout << "\tr = recognise current image" << std::endl;
    std::cout << "\tany key = clear recognition result" << std::endl;
    std::cout << "\tx = exit" << std::endl;
}

#define MAX_NUMBER_OF_SAMPLE_IMAGES 25

/******************************************************************************/

int main( int argc, char** argv )
{

    Mat img;                  // image objects
    VideoCapture cap; 		// capture object

    const string windowName = ".... Recognition"; // window name

    bool keepProcessing = true;	// loop control flag
    int key;						// user input
    int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
    // 40 ms equates to 1000ms/25fps = 40ms per frame

    // data structures and matrices for image based recognition

    Mat input[MAX_NUMBER_OF_SAMPLE_IMAGES];

    int imagesCollected = 0;			// number of sample images collected

    bool recognitionStage = false;	// flag to determine when have started
    // recognition
    int closestImage = 0;             // index of best match

    // if command line arguments are provided try to read video_name
    // otherwise default to capture from attached H/W camera

    if(( argc == 2 && (cap.open(argv[1]) == true )) || ( argc != 2 && (cap.open(0) == true)))
    {
        // print user controls

        printhelp();

        // create window object (use flag=0 to allow resize, 1 to auto fix size)

        namedWindow(windowName, 0);

        // start main loop

        while (keepProcessing)
        {
            int64 timeStart = getTickCount(); // get time at start of loop

            // if capture object in use (i.e. video/camera)
            // get image from capture object

            if (cap.isOpened())
            {

                cap >> img;
                if(img.empty())
                {
                    if (argc == 2)
                    {
                        std::cerr << "End of video file reached" << std::endl;
                    }
                    else
                    {
                        std::cerr << "ERROR: cannot get next fram from camera"
                                  << std::endl;
                    }
                    exit(0);
                }

            }

            // ***** DO ANY PRE-DISPLAY PROCESSING HERE ****

            // display image in window (with text)

            if (!recognitionStage)
            {
                putText(img, "SAMPLE COLLECTION", Point(10,img.rows - 10),
                        FONT_HERSHEY_PLAIN, 2.0, CV_RGB(0, 255,0), 3, 8, false);
            }
            else
            {
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

            if (key == 'x')
            {

                // if user presses "x" then exit

                std::cout << "Keyboard exit requested : exiting now - bye!"
                          << std::endl;
                keepProcessing = false;

            }
            else if (key == ' ')
            {

                // if user presses " " then capture a sample image

                if (!recognitionStage)
                {
                    if (imagesCollected < MAX_NUMBER_OF_SAMPLE_IMAGES)
                    {

                        // copy image and store it

                        input[imagesCollected] = img.clone();
                        imagesCollected++;

                        std::cout << "Sample image collected - " <<
                                  imagesCollected << std::endl;


                        // ***** DO ANY OTHER PREPROCESSING / INFO. EXTRACTION HERE *****

                    }
                    else
                    {
                        std::cout <<  "ERROR: Maximum sample images (" <<
                                  imagesCollected << ") collected" << std::endl;
                    }
                }

            }
            else if ((key == '\n') || (key == 'm'))     // use "m" in windows
            {

                // if user presses return then move into recognition mode

                std::cout << "Entering recognition mode - ..." << std::endl << std::endl;

                recognitionStage = true;
                if (!(imagesCollected > 0))
                {
                    std::cerr << "ERROR: not enough samples images caputured" <<
                              std::endl;
                }
            }
            else if (key == 'r')
            {

                // if user presses "r" then do recognition

                // *********** EXTRACT INFORMATION FROM THE CURRENT INPUT IMAGE HERE

                if (recognitionStage)
                {


                    for (int i = 0; i < imagesCollected; i++)
                    {

                        // *********** MATCH INFORMATION TO STORED IMAGES HERE

                        // set closestImage as the index of the best matching
                        // image in the input array of images

                        closestImage = 0;

                    }

                    // output the result in a window

                    std::cout << "Recognition - closest matching image = " <<
                              closestImage << std::endl;
                    std::cout <<  "Press any key to clear." << std::endl << std::endl;

                    namedWindow("Recognition Result", 1 );
                    imshow("Recognition Result", input[closestImage]);
                    waitKey(0);
                    destroyWindow("Recognition Result"); // close window

                }
                else
                {
                    std::cout << "ERROR - need to enter recognition stage first."
                              << std::endl;
                }
            }
        }


        // all images should be killed off by their respective destructors

        // the camera will be deinitialized automatically in VideoCapture destructor

        // all OK : main returns 0

        return 0;
    }

    // not OK : main returns -1

    return -1;
}
/******************************************************************************/
