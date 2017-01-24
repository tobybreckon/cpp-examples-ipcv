// Example : saving an image
// usage: prog <image_name> <output_img>

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

  Mat inputImg;			// input image object
  Mat outputImg;		// output image object
  std::vector<int> params;	// file saving compression parameters

  // check that command line arguments are provided and image reads in OK

    if ((argc == 3) && !(inputImg = imread( argv[1], CV_LOAD_IMAGE_COLOR)).empty())
    {

      // invert image_name

	  bitwise_not(inputImg, outputImg);

	  // write out image to file

	  params.push_back(CV_IMWRITE_JPEG_QUALITY);
 	  params.push_back(95);

	  imwrite(argv[2], outputImg, params);

      // all OK : main returns 0

      return 0;
    }

    // not OK : main returns -1

    return -1;
}
