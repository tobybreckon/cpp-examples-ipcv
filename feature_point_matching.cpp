// Example : feature point matching and homography calculation from camera or video
// usage: prog {<video_name>}

// Author : Toby Breckon, toby.breckon@durham.ac.uk

// Copyright (c) 2011 School of Engineering, Cranfield University
// Copyright (c) 2017 Dept. of Computer Science, Durham University

// requires OpenCV 3.x (or greater) + ** built with extra contrib modules **

// License : LGPL - http://www.gnu.org/licenses/lgpl.html

#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <iostream>     // standard C++ I/O
#include <string>       // standard C++ I/O
#include <algorithm>    // includes max()

using namespace cv; // OpenCV API is in the C++ "cv" namespace
using namespace cv::xfeatures2d;
using namespace std;

/******************************************************************************/
// setup the cameras properly based on OS platform

// 0 in linux gives first camera for v4l
//-1 in windows gives first device or user dialog selection

#ifdef linux
    #define CAMERA_INDEX  0
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

//Copy (x,y) location of descriptor matches found from KeyPoint data structures into Point2f vectors
void matches2points(const vector<vector<DMatch> >& matches, const vector<KeyPoint>& kpts_train,
  const vector<KeyPoint>& kpts_query, vector<Point2f>& pts_train,
  vector<Point2f>& pts_query)
{
    pts_train.clear();
    pts_query.clear();

    for (size_t k = 0; k < matches.size(); k++)
    {
        for (size_t i = 0; i < matches[k].size(); i++)
        {
            const DMatch& match = matches[k][i];
            pts_query.push_back(kpts_query[match.queryIdx].pt);
            pts_train.push_back(kpts_train[match.trainIdx].pt);
        }
    }

}


/******************************************************************************/

int main( int argc, char** argv )
{

    Mat img, roi, selected, gray, graySelected, output, selectedCopy, transformOverlay; // image objects
    VideoCapture cap; // capture object

    const string windowName = "Live Video Input"; // window name
    const string windowName2 = "Selected Region / Object"; // window name
    const string windowName3 = "Matches"; // window name


    vector<KeyPoint> keypointsVideo;        // keypoints and descriptors
    vector<KeyPoint> keypointsSelection;
    Mat descSelection, descVideo;
    // vector<DMatch> matches_internal;
    vector<vector<DMatch> > matches;
    vector<Mat> training;
    vector<Point2f> detectedPointsVideo;
    vector<Point2f> detectedPointsSelection;

    bool keepProcessing = true; // loop control flag
    int key;                    // user input
    int EVENT_LOOP_DELAY = 40;  // delay for GUI window
                                // 40 ms equates to 1000ms/25fps = 40ms per frame

    // SURF feature detector with Hessian threshold: 400 using 4 octaves over 3 layers
    // (the default parameters - see manual for details)

    Ptr<Feature2D> detector = SURF::create();

    // DescriptorExtractor *extractor = new SurfDescriptorExtractor(); // extracts descriptors of
    // the detected points
    FlannBasedMatcher matcher;              // descriptor matcher (k-NN based)
                                            // (FLANN = Fast Library for Approximate Nearest Neighbors)

    int threshold = 10;                     // matching threshold

    Mat H, H_prev;                           // image to image homography (transformation)

    bool showEllipse = false;               // stuff for drawing the ellipse
    RotatedRect ellipseFit;
    bool drawLivePoints = false;
    bool newFeatureType = false;
    bool computeHomography = false;

    // set up the features - here using only SURF, SIFT and KAZE that use floating
    // point descriptors by default as the others - ORB, BRISK, FREAK, AKAZE all
    // require more messing arounf with a type conversion of the binary descriptors
    // to floating point for use of FLANN matching and/or matching in L1 space
    //
    // Although this is fully possible within OpenCV, for example see:
    // On using Feature Descriptors as Visual Words for Object Detection within
    // X-ray Baggage Security Screening (M.E. Kundegorski, S. Akcay, M. Devereux,
    // A. Mouton, T.P. Breckon), In Proc. International Conference on Imaging for
    // Crime Detection and Prevention, IET, pp. 12 (6 .)-12 (6 .)(1), 2016.

    enum feature_types { SURF, SIFT, KAZE };
    int feature_types_max = 3;
    float match_threshold_multipliers[] = {0.01, 10, 0.01};
    int currentFeatureType = SURF;
    float threshold_multiplier = match_threshold_multipliers[currentFeatureType];

    // matches.push_back(matches_internal);

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
        namedWindow(windowName3, 0);
        setMouseCallback( windowName, onMouseSelect, &img);
        createTrackbar("threshold (* 0.01)", windowName3, &threshold, 200, 0);

        std::cout << "'e' - toggle ellipse fit for detected points (default: off)" << std::endl;
        std::cout << "'p' - toggle drawing for live feature points (default: off)" << std::endl;
        std::cout << "'t' - toggle use of varying points & descriptors (default: SURF)" << std::endl;
        std::cout << "'h' - compute image to image homography (default: off)" << std::endl;

        // start main loop

        while (keepProcessing) {

            int64 timeStart = getTickCount(); // get time at start of loop

            // if capture object in use (i.e. video/camera)
            // get image from capture object

            if (cap.isOpened()) {

                cap >> img;
                if(img.empty()) {
                    if (argc == 2) {
                        std::cerr << "End of video file reached" << std::endl;
                    } else {
                        std::cerr << "ERROR: cannot get next frame from camera"
                                  << std::endl;
                    }
                    exit(0);
                }
            }

            // convert incoming image to grayscale

            cvtColor(img, gray, CV_BGR2GRAY);

            // detect the feature points from the current incoming frame and extract
            // corresponding descriptors

            keypointsVideo.clear();
            detector->detect(gray, keypointsVideo);
            detector->compute(gray, keypointsVideo, descVideo);

            // match descriptors to selection (if we have a selected object)

            if (!(descSelection.empty()))
            {
                if (threshold == 0) {threshold = 1; }
                matches.clear();
                matcher.radiusMatch(descVideo, matches, (float) (threshold) * threshold_multiplier);

                // draw results on image

                output = Mat::zeros(img.rows, img.cols + selected.cols, img.type());
                drawMatches(gray, keypointsVideo, graySelected, keypointsSelection, matches, output,
                  Scalar(0,255,0), Scalar(-1,-1,-1), vector<vector<char> >(),
                  DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

                // get the matches as points in both images

                matches2points(matches, keypointsSelection, keypointsVideo, detectedPointsSelection, detectedPointsVideo);

                // fit ellipse to object location

                if (showEllipse)
                {
                    if (detectedPointsSelection.size() > 6)
                    {
                        ellipseFit = fitEllipse(Mat(detectedPointsVideo));
                        ellipse(output, ellipseFit, Scalar(0, 0, 255), 2, 8);
                    }
                }

                // compute and display homography (mapping on image to another) if selected

                if (computeHomography && (detectedPointsSelection.size() > 5))
                {
                    // need at least 5 matched pairs of points (more are better)

                    Mat H = findHomography(Mat(detectedPointsSelection), Mat(detectedPointsVideo), RANSAC, 2);
                    transformOverlay = Mat::zeros(output.rows, output.cols, output.type());
                    warpPerspective(selectedCopy, transformOverlay, H, transformOverlay.size(), INTER_LINEAR, BORDER_CONSTANT, 0);
                    addWeighted(output, 0.5, transformOverlay, 0.5, 0, output);
                }

                // display result

                imshow(windowName3, output);

            }

            // whist we are selecting or have selected an object

            if( selectObject && selection.width > 0 && selection.height > 0 )
            {

                // invert selection in image whilst selection is taking place

                roi = img(selection);
                bitwise_not(roi, roi);

            } else if ((selectionComplete || newFeatureType) && selection.width > 0 && selection.height > 0 ) {

                // once it is complete then make a copy of the selection and extract descriptors
                // from detected points

                if (!newFeatureType) {
                    selected = roi.clone();
                    selectedCopy  = roi.clone();
                    cvtColor(selected, graySelected, CV_BGR2GRAY);
                } else {
                    selected = selectedCopy.clone();
                }

                newFeatureType = false;
                selectionComplete = false;

                keypointsSelection.clear();
                detector->detect(graySelected, keypointsSelection);
                detector->compute(graySelected, keypointsSelection, descSelection);

                // draw the features

                drawKeypoints(graySelected, keypointsSelection, selected,
                  Scalar(255, 0, 0), DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                // train the matcher on this example

                training.clear();
                training.push_back(descSelection);
                matcher.clear();
                matcher.add(training);
                matcher.train();

            }

            // display image in window (with keypoints if activated)

            if (drawLivePoints)
            {
                drawKeypoints(gray, keypointsVideo, img,
                  Scalar(255, 0, 0), DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            }

            imshow(windowName, img);
            if (!(selected.empty()))
            {
                imshow(windowName2, selected);
            }

            // start event processing loop (very important,in fact essential for GUI)
            // 40 ms roughly equates to 1000ms/25fps = 4ms per frame

            // here we take account of processing time for the loop by subtracting the time
            // taken in ms. from this (1000ms/25fps = 40ms per frame) value whilst ensuring
            // we get a +ve wait time

            key = waitKey((int) std::max(2.0, EVENT_LOOP_DELAY -
                (((getTickCount() - timeStart) / getTickFrequency()) * 1000)));

            switch (key)
            {
            case 'x':
                // if user presses "x" then exit

                std::cout << "Keyboard exit requested : exiting now - bye!" << std::endl;
                keepProcessing = false;
                break;
            case 'p':
                // if user presses "p" live feature point drawing

                drawLivePoints = (!drawLivePoints);
                std::cout << "feature point drawing (live) = " << drawLivePoints << std::endl;
                break;
            case 'e':
                // if user presses "e" toggle ellipse drawing

                showEllipse = (!showEllipse);
                std::cout << "Ellipse drawing = " << showEllipse << std::endl;
                break;
            case 't':
                // toggle feature point type

                currentFeatureType++;
                currentFeatureType = currentFeatureType % feature_types_max;

                switch (currentFeatureType)
                {
                case SURF:
                    detector = SURF::create();
                    std::cout << "now using SURF" << std::endl;
                    break;
                case SIFT:
                    detector = SIFT::create();
                    std::cout << "now using SIFT" << std::endl;
                    break;
                case KAZE:
                    detector = KAZE::create();
                    std::cout << "now using KAZE" << std::endl;
                    break;

                }

                // reset selection and set a new type flag

                newFeatureType = true;
                descSelection = Mat();

                // get suitable matching threshold multiplier for slider

                threshold_multiplier = match_threshold_multipliers[currentFeatureType];

                break;
            case 'h':

                // compute and display homography transformation from one image to the other

                computeHomography = (!computeHomography);
                std::cout << "Compute homography = " << computeHomography << std::endl;

                break;
            default:
                break;
            }

        }

        // pointer objects auto deleted as smart pointers Ptr<>

        // the camera will be deinitialized automatically in VideoCapture destructor

        // all OK : main returns 0

        return 0;
    }

    // not OK : main returns -1

    return -1;
}

/******************************************************************************/
