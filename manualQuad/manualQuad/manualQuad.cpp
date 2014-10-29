#include <vector>
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <list>

#include <opencv2/video/background_segm.hpp>
#include <opencv2/legacy/blobtrack.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>

#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;
using namespace cv;

//=============== VARIABLES ========================================//
int xVal;
int yVal;
int xHover;
int yHover;
bool lClicked = false;
bool rClicked = false;
Point2f tl;
Point2f tr;
Point2f bl;
Point2f br;
Point2f center;
Mat imgOriginal;
Mat imgBuffer;
Mat quad = Mat::zeros(500, 500, CV_8UC3);
vector<Point2f> corners;
//=============== variables ========================================//

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "\n\n\nSET\n\n\n";
        xVal = x;
        yVal = y;
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        //cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
        else if  ( event == EVENT_MBUTTONDOWN )
    {
        //cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if ( event == EVENT_MOUSEMOVE )
    {
        //cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
        xHover = x;
        yHover = y;
    }
}

int main(int argc, char** argv)
{
    VideoCapture cap(1); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }

//============================== OBJECT CONTROL =====================================================//
    namedWindow("Object", CV_WINDOW_AUTOSIZE); //create a window called "Object"

    int iLowH = 0;
    int iHighH = 10;

    int iLowS = 135;
    int iHighS = 255;

    int iLowV = 50;
    int iHighV = 255;

    //Create trackbars in "Object" window
    createTrackbar("LowH", "Object", &iLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Object", &iHighH, 179);

    createTrackbar("LowS", "Object", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Object", &iHighS, 255);

    createTrackbar("LowV", "Object", &iLowV, 255);//Value (0 - 255)
    createTrackbar("HighV", "Object", &iHighV, 255);
//============================== object control =====================================================//


//============================== SETUP CORNERS ======================================================//

    cout << "Setup top left...\n Press any key to start, press esc when done \n\n";
    waitKey();

    while(1)
    {
        cap.read(imgOriginal);

        //Create a window
        namedWindow("Camera", 1);

        //set the callback function for any mouse event
        setMouseCallback("Camera", CallBackFunc, NULL);

        // show clicked value
        cout << "\t" << xHover << "\t" << yHover << endl;
        tl.x = xVal;
        tl.y = yVal;

        //show the image
        imshow("Camera", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 1ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed, top left set" << endl;
            cout << "top left: " << tl << "\n\n";
                break;
        }
    }

    cout << "Setup top right...\n Press any key to start, press esc when done \n\n";
    waitKey();

    while(1)
    {
        cap.read(imgOriginal);

        //Create a window
        namedWindow("Camera", 1);

        //set the callback function for any mouse event
        setMouseCallback("Camera", CallBackFunc, NULL);

        // show clicked value
        cout << "\t" << xHover << "\t" << yHover << endl;
        tr.x = xVal;
        tr.y = yVal;

        //show the image
        imshow("Camera", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 1ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed, top right set" << endl;
            cout << "top right: " << tr << "\n\n";
                break;
        }
    }

    cout << "Setup bottom left...\n Press any key to start, press esc when done \n\n";
    waitKey();

    while(1)
    {
        cap.read(imgOriginal);

        //Create a window
        namedWindow("Camera", 1);

        //set the callback function for any mouse event
        setMouseCallback("Camera", CallBackFunc, NULL);

        // show clicked value
        cout << "\t" << xHover << "\t" << yHover << endl;
        bl.x = xVal;
        bl.y = yVal;

        //show the image
        imshow("Camera", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 1ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed, bottom left set" << endl;
            cout << "bottom left: " << bl << "\n\n";
                break;
        }
    }

    cout << "Setup bottom right...\n Press any key to start, press esc when done \n\n";
    waitKey();

    while(1)
    {
        cap.read(imgOriginal);

        //Create a window
        namedWindow("Camera", 1);

        //set the callback function for any mouse event
        setMouseCallback("Camera", CallBackFunc, NULL);

        // show clicked value
        cout << "\t" << xHover << "\t" << yHover << endl;
        br.x = xVal;
        br.y = yVal;

        //show the image
        imshow("Camera", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 1ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed, bottom right set" << endl;
            cout << "bottom right: " << br << "\n\n";
                break;
        }
    }
//============================== setup corners ======================================================//


    // get mass center
    center.x = (tl.x + tr.x + bl.x + br.x) / 4;
    center.y = (tl.y + tr.y + bl.y + br.y) / 4;

    // input corners to array corners
    corners.clear();
    corners.push_back(tl);
    corners.push_back(tr);
    corners.push_back(br);
    corners.push_back(bl);

    waitKey();
    while(1)
    {

//================================== DRAW AREA ======================================================//

        cap.read(imgOriginal);

        // Draw circles on corner and center
        circle( imgOriginal, tl, 3.0, Scalar( 0, 0, 255), 3, 8 );
        circle( imgOriginal, tr, 3.0, Scalar( 0, 0, 255), 3, 8 );
        circle( imgOriginal, bl, 3.0, Scalar( 0, 0, 255), 3, 8 );
        circle( imgOriginal, br, 3.0, Scalar( 0, 0, 255), 3, 8 );
        circle( imgOriginal, center, 3.0, Scalar( 0, 0, 255), 3, 8 );

        // Draw lines
        line(imgOriginal, tl, tr, CV_RGB(0,255,0));
        line(imgOriginal, tl, bl, CV_RGB(0,255,0));
        line(imgOriginal, br, tr, CV_RGB(0,255,0));
        line(imgOriginal, br, bl, CV_RGB(0,255,0));

//================================== draw area ======================================================//


//============================== GET PRESPECTIVE ====================================================//

        cap.read(imgBuffer);

        vector<Point2f> quad_pts;
        quad_pts.push_back(Point2f(0, 0));
        quad_pts.push_back(Point2f(quad.cols, 0));
        quad_pts.push_back(Point2f(quad.cols, quad.rows));
        quad_pts.push_back(Point2f(0, quad.rows));
        Mat transmtx = getPerspectiveTransform(corners, quad_pts);
        warpPerspective(imgBuffer, quad, transmtx, quad.size());

//============================== get prespective ====================================================//

//==================== OBJECT DETECTION ===========================================================================//
        Mat imgHSV;
        cvtColor(quad, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

        //morphological opening (removes small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //morphological closing (removes small holes from the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //Calculate the moments of the thresholded image
        Moments oMoments = moments(imgThresholded);

        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
        int posX, posY;

        // if the area <= 10000, I consider that the there are no object in the image
        //and it's because of the noise, the area is not zero
        if (dArea > 10000)
        {
            //calculate the position of the ball
            posX = dM10 / dArea;
            posY = dM01 / dArea;

            // Draw a circle
            circle( quad, Point(posX,posY), 16.0, Scalar( 0, 0, 255), 3, 8 );

            cout << posX << "\t";
            cout << posY << "\n\n";
        }
//==================== object detection ===========================================================================//


//========================== CREATE AND SHOW IMAGE ==================================================//

        //Create a window
        //namedWindow("Camera", 1);

        //show the image
        imshow("Camera", imgOriginal);

        //Create a window
        //namedWindow("Perspective", 1);

        //show the image
        imshow("Perspective", quad);
        imshow("Thresholded Image", imgThresholded); //show the thresholded image

//========================== create and show image ==================================================//



        if (waitKey(1) == 27) //wait for 'esc' key press for 1ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed" << endl;
                break;
        }
    }
    return 0;

}
