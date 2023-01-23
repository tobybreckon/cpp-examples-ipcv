// Example : Apply Non-Local Means (NLM) image / video / camera
// usage: prog {<image_name> | <video_name>}

// Author : Toby Breckon, toby.breckon@durham.ac.uk

// Copyright (c) 2010 School of Engineering, Cranfield University
// Copyright (c) 2016 School of Engineering & Computing Sciences, Durham University
// License : LGPL - http://www.gnu.org/licenses/lgpl.html

#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"

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
	#define CAMERA_INDEX 1
#else
	#define CAMERA_INDEX -1
#endif

/******************************************************************************/

// Non Local Mean - lifted directly from: http://opencv.jp/opencv2-x-samples/non-local-means-filter
// Code Credit: @fukushima1981(Twitter)
// Code provided "as is" from original source

// Reference:
// A. Buades, B. Coll, J.M. Morel “A non local algorithm for image denoising”
// IEEE Computer Vision and Pattern Recognition 2005, Vol 2, pp: 60-65, 2005.

static void nonlocalMeansFilter(Mat& src, Mat& dest, int templeteWindowSize,
                                int searchWindowSize, double h, double sigma=0.0)
{
    if(templeteWindowSize>searchWindowSize)
    {
        std::cout<<"searchWindowSize should be larger than templeteWindowSize"<<std::endl;
        return;
    }
    if(dest.empty())dest=Mat::zeros(src.size(),src.type());

    const int tr = templeteWindowSize>>1;
    const int sr = searchWindowSize>>1;
    const int bb = sr+tr;
    const int D = searchWindowSize*searchWindowSize;
    const int H=D/2+1;
    // const double div = 1.0/(double)D;//search area div
    const int tD = templeteWindowSize*templeteWindowSize;
    const double tdiv = 1.0/(double)(tD);//templete square div

    //create large size image for bounding box;
    Mat im;
    copyMakeBorder(src,im,bb,bb,bb,bb,cv::BORDER_DEFAULT);

    //weight computation;
    vector<double> weight(256*256*src.channels());
    double* w = &weight[0];
    const double gauss_sd = (sigma == 0.0) ? h :sigma;
    double gauss_color_coeff = -(1.0/(double)(src.channels()))*(1.0/(h*h));
    int emax = INT_MAX;
    for(int i = 0; i < 256*256*src.channels(); i++ )
    {
        double v = std::exp( max(i-2.0*gauss_sd*gauss_sd,0.0)*gauss_color_coeff);
        w[i] = v;
        if(v<0.001)
        {
            emax=i;
            break;
        }
    }
    for(int i = emax; i < 256*256*src.channels(); i++ )w[i] = 0.0;

    if(src.channels()==3)
    {
        const int cstep = im.step-templeteWindowSize*3;
        const int csstep = im.step-searchWindowSize*3;
#pragma omp parallel for
        for(int j=0;j<src.rows;j++)
        {
            uchar* d = dest.ptr(j);
            int* ww=new int[D];
            double* nw=new double[D];
            for(int i=0;i<src.cols;i++)
            {
                double tweight=0.0;
                //search loop
                uchar* tprt = im.data +im.step*(sr+j) + 3*(sr+i);
                uchar* sptr2 = im.data +im.step*j + 3*i;
                for(int l=searchWindowSize,count=D-1;l--;)
                {
                    uchar* sptr = sptr2 +im.step*(l);
                    for (int k=searchWindowSize;k--;)
                    {
                        //templete loop
                        int e=0;
                        uchar* t = tprt;
                        uchar* s = sptr+3*k;
                        for(int n=templeteWindowSize;n--;)
                        {
                            for(int m=templeteWindowSize;m--;)
                            {
                                // computing color L2 norm
                                e += (s[0]-t[0])*(s[0]-t[0])+(s[1]-t[1])*(s[1]-t[1])+(s[2]-t[2])*(s[2]-t[2]);//L2 norm
                                s+=3,t+=3;
                            }
                            t+=cstep;
                            s+=cstep;
                        }
                        const int ediv = e*tdiv;
                        ww[count--]=ediv;
                        //get weighted Euclidean distance
                        tweight+=w[ediv];
                    }
                }
                //weight normalization
                if(tweight==0.0)
                {
                    for(int z=0;z<D;z++) nw[z]=0;
                    nw[H]=1;
                }
                else
                {
                    double itweight=1.0/(double)tweight;
                    for(int z=0;z<D;z++) nw[z]=w[ww[z]]*itweight;
                }

                double r=0.0,g=0.0,b=0.0;
                uchar* s = im.ptr(j+tr); s+=3*(tr+i);
                for(int l=searchWindowSize,count=0;l--;)
                {
                    for(int k=searchWindowSize;k--;)
                    {
                        r += s[0]*nw[count];
                        g += s[1]*nw[count];
                        b += s[2]*nw[count++];
                        s+=3;
                    }
                    s+=csstep;
                }
                d[0] = saturate_cast<uchar>(r);
                d[1] = saturate_cast<uchar>(g);
                d[2] = saturate_cast<uchar>(b);
                d+=3;
            }//i
            delete[] ww;
            delete[] nw;
        }//j
    }
    else if(src.channels()==1)
    {
        const int cstep = im.step-templeteWindowSize;
        const int csstep = im.step-searchWindowSize;
#pragma omp parallel for
        for(int j=0;j<src.rows;j++)
        {
            uchar* d = dest.ptr(j);
            int* ww=new int[D];
            double* nw=new double[D];
            for(int i=0;i<src.cols;i++)
            {
                double tweight=0.0;
                //search loop
                uchar* tprt = im.data +im.step*(sr+j) + (sr+i);
                uchar* sptr2 = im.data +im.step*j + i;
                for(int l=searchWindowSize,count=D-1;l--;)
                {
                    uchar* sptr = sptr2 +im.step*(l);
                    for (int k=searchWindowSize;k--;)
                    {
                        //templete loop
                        int e=0;
                        uchar* t = tprt;
                        uchar* s = sptr+k;
                        for(int n=templeteWindowSize;n--;)
                        {
                            for(int m=templeteWindowSize;m--;)
                            {
                                // computing color L2 norm
                                e += (*s-*t)*(*s-*t);
                                s++,t++;
                            }
                            t+=cstep;
                            s+=cstep;
                        }
                        const int ediv = e*tdiv;
                        ww[count--]=ediv;
                        //get weighted Euclidean distance
                        tweight+=w[ediv];
                    }
                }
                //weight normalization
                if(tweight==0.0)
                {
                    for(int z=0;z<D;z++) nw[z]=0;
                    nw[H]=1;
                }
                else
                {
                    double itweight=1.0/(double)tweight;
                    for(int z=0;z<D;z++) nw[z]=w[ww[z]]*itweight;
                }

                double v=0.0;
                uchar* s = im.ptr(j+tr); s+=(tr+i);
                for(int l=searchWindowSize,count=0;l--;)
                {
                    for(int k=searchWindowSize;k--;)
                    {
                        v += *(s++)*nw[count++];
                    }
                    s+=csstep;
                }
                 *(d++) = saturate_cast<uchar>(v);
            }//i
            delete[] ww;
            delete[] nw;
        }//j
    }
}

/******************************************************************************/

int main( int argc, char** argv )
{

  Mat img, output;	// image objects
  VideoCapture cap; // capture object

  const string windowName = "Original"; // window name
  const string windowName2 = "Non Local Means Filter"; // window name

  bool keepProcessing = true;	// loop control flag
  unsigned char  key;			// user input
  int  EVENT_LOOP_DELAY = 40;	// delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

  int64 pre = 0;                // timing variable

  // NLM parameters

  int templateWindowSize = 3;
  int searchWindowSize = 7;
  int h = 3;
  int hc = 10;

  // check which version of OpenCV we are using

  std::cout << "OpenCV version" << CV_VERSION
                    << " (" << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << ")" << std::endl;


  // if command line arguments are provided try to read image/video_name
  // otherwise default to capture from attached H/W camera

    if(
	  ( argc == 2 && (!(img = imread( argv[1], IMREAD_COLOR)).empty()))||
	  ( argc == 2 && (cap.open(argv[1]) == true )) ||
	  ( argc != 2 && (cap.open(CAMERA_INDEX) == true))
	  )
    {
      // create window object (use flag=0 to allow resize, 1 to auto fix size)

      namedWindow(windowName, 1);
      namedWindow(windowName2, 1);

      // add trackbars

        createTrackbar("template W", windowName2, &templateWindowSize, 25);
        createTrackbar("search W", windowName2, &searchWindowSize, 50);
        createTrackbar("h", windowName2, &h, 25);
        createTrackbar("hc", windowName2, &hc, 25);

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

          if (searchWindowSize <= templateWindowSize)
          {
              std::cout << "ERROR: search W must be > template W (setting search W = (template W) + 1)" << std::endl;
              searchWindowSize = templateWindowSize + 1;
          }

          pre = getTickCount();

          #if ((CV_MAJOR_VERSION >= 2) && (CV_MINOR_VERSION >= 4) && (CV_SUBMINOR_VERSION <= 2))

            // in OpenCV version 2.4.2 and earlier we use this version

            nonlocalMeansFilter(img,output, templateWindowSize, searchWindowSize, (double) h, (double) h);

          #else

            // use version built-in to later versions of OpenCV

            if (img.channels() == 3) // if RGB then use colour function on L*a*b colour space (see manual)
            {
                fastNlMeansDenoisingColored(img, output, h, hc, templateWindowSize, searchWindowSize);
            } else {
                fastNlMeansDenoising(img, output, h, templateWindowSize, searchWindowSize);
            }
            // Reference:
            // A. Buades, B. Coll, J.M. Morel “A non local algorithm for image denoising”
            // IEEE Computer Vision and Pattern Recognition 2005, Vol 2, pp: 60-65, 2005.

          #endif

          std::cout << "time: " << 1000.0*(getTickCount()-pre)/(getTickFrequency()) << " ms" <<  std::endl;

		  // display image in window

		  imshow(windowName, img);
          imshow(windowName2, output);

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
