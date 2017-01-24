// Example :  using openCV polygon functions in C++

// Author : Toby Breckon, toby.breckon@durham.ac.uk

// Copyright (c) 2011 School of Engineering, Cranfield University
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

int main( int argc, char** argv )
{
	// create a RGB colour image (set it to a black background)

  	Mat img = Mat::zeros(400, 400, CV_8UC3);

	// define a polygon (as a vector of points)

	vector<Point> contour;
	contour.push_back(Point(50,50));
	contour.push_back(Point(300,50));
	contour.push_back(Point(350,200));
	contour.push_back(Point(300,150));
	contour.push_back(Point(150,350));
	contour.push_back(Point(100,100));

	// create a pointer to the data as an array of points (via a conversion to
	// a Mat() object)

	const cv::Point *pts = (const cv::Point*) Mat(contour).data;
	int npts = Mat(contour).rows;

	std::cout << "Number of polygon vertices: " << npts << std::endl;

	// draw the polygon

	polylines(img, &pts,&npts, 1,
	    		true, 			// draw closed contour (i.e. joint end to start)
	            Scalar(0,255,0),// colour RGB ordering (here = green)
	    		3, 		        // line thickness
			    CV_AA, 0);


	// do point in polygon test (by conversion/cast to a Mat() object)
	// define and test point one (draw it in red)

	Point2f test_pt;
	test_pt.x = 150;
	test_pt.y = 75;

	rectangle(img, test_pt, test_pt, Scalar(0, 0, 255), 3, 8, 0); // RED point

	if (pointPolygonTest(Mat(contour), test_pt, true) > 0){
		std::cout << "RED {" << test_pt.x << "," << test_pt.y
				  << "} is in the polygon (dist. "
				  << pointPolygonTest(Mat(contour), test_pt, 1) << ")"
			      << std::endl;
	}

	// define and test point two (draw it in blue)

	test_pt.x = 50;
	test_pt.y = 350;

	rectangle(img, test_pt, test_pt, Scalar(255, 0, 0), 3, 8, 0); // BLUE point

	if (pointPolygonTest(Mat(contour), test_pt, true) < 0){
		std::cout << "BLUE {" << test_pt.x << "," << test_pt.y
				  << "} is NOT in the polygon (dist. "
				  << pointPolygonTest(Mat(contour), test_pt, 1) << ")"
			      << std::endl;
	}

	// pointPolygonTest :-
	// "The function determines whether the point is inside a contour, outside,
	// or lies on an edge (or coincides with a vertex). It returns positive
	// (inside), negative (outside) or zero (on an edge) value, correspondingly.
	// When measureDist=false , the return value is +1, -1 and 0, respectively.
	// Otherwise, the return value it is a signed distance between the point
	// and the nearest contour edge." - OpenCV Manual version 2.1

	// create an image and display the image

  	namedWindow("Polygon Test", 0);
	imshow( "Polygon Test", img );
	waitKey(0);

    return 0;
}

/******************************************************************************/
