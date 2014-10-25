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

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    VideoCapture cap(1); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }

    //=============== OBJECT CONTROL ==============================================//
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
    //=============== object control ==============================================//


    //=============== FIELD CONTROL ==============================================//
    namedWindow("Field", CV_WINDOW_AUTOSIZE); //create a window called "Field"

    int fLowH = 65;
    int fHighH = 100;

    int fLowS = 90;
    int fHighS = 220;

    int fLowV = 60;
    int fHighV = 130;

    //Create trackbars in "Field" window
    createTrackbar("LowH", "Field", &fLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Field", &fHighH, 179);

    createTrackbar("LowS", "Field", &fLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Field", &fHighS, 255);

    createTrackbar("LowV", "Field", &fLowV, 255);//Value (0 - 255)
    createTrackbar("HighV", "Field", &fHighV, 255);
    //=============== field control ==============================================//

    //Capture a temporary image from the camera
    Mat imgTmp;
    cap.read(imgTmp);

    //Create a black image with the size as the camera output
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;


    while (true)
    {
        Mat imgOriginal;
        bool bSuccess = cap.read(imgOriginal); // read a new frame from video

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        Mat imgHSV;
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        //==================== OBJECT DETECTION ===========================================================================//
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
            circle( imgOriginal, Point(posX,posY), 16.0, Scalar( 0, 0, 255), 3, 8 );

            cout << posX << "\t";
            cout << posY << "\n\n";
        }
        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        //==================== object detection ===========================================================================//

        //==================== FIELD DETECTION ===========================================================================//
        Mat imgField;
        inRange(imgHSV, Scalar(fLowH, fLowS, fLowV), Scalar(fHighH, fHighS, fHighV), imgField); //Threshold the image

        //morphological opening (removes small objects from the foreground)
        erode(imgField, imgField, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgField, imgField, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //morphological closing (removes small holes from the foreground)
        dilate( imgField, imgField, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(imgField, imgField, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        Mat imgGray;
        blur(imgField,imgGray,Size(3, 3));
        Canny(imgGray,imgGray,100,100,3); //Get edge map
        vector<Vec4i> lines;
        HoughLinesP(imgGray, lines, 1, CV_PI/180, 70, 30, 10);

        // Draw lines
        for (int i = 0; i < lines.size(); i++)
        {
            Vec4i v = lines[i];
            line(imgOriginal, Point(v[0], v[1]), Point(v[2], v[3]), CV_RGB(0,255,0));
        }
        //imshow("Field Image", imgField); //show the thresholded image
        //==================== field detection ===========================================================================//

        //imshow("Edge Map", imgGray); //show the edge map
        //imgOriginal = imgOriginal + imgLines;
        imshow("Original", imgOriginal); //show the original image

            if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
            {
            cout << "esc key is pressed by user" << endl;
                    break;
            }
        }
    return 0;
}
