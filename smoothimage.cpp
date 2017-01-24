// Example : smooth an image
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


int main( int argc, char** argv )
{

  Mat inputImg;  // input image object
  Mat outputImg; // output image object

  const string windowName = "OPENCV: blurred image"; // window name

  // check that command line arguments are provided and image reads in OK

    if ((argc == 2) && !(inputImg = imread( argv[1], CV_LOAD_IMAGE_COLOR)).empty())
    {

      // blur the input image using a 5 x 5 mask and store in output image
      // (The output image will be created automatically)

      GaussianBlur(inputImg, outputImg, Size(5, 5), 0, 0, BORDER_DEFAULT);

      // create window object

      namedWindow(windowName, 1 );

      // display image in window

      imshow(windowName, outputImg );

      // start event processing loop (very important,in fact essential for GUI)

      waitKey(0);

      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
