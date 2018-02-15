//
//  main.cpp
//  opencv
//
//  Created by Sebastian Coronado on 12/02/18.
//  Copyright © 2018 Sebastian Coronado. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

const Size chessboardDimension = Size(5,4);

void createMarker()
{
    Mat outputMarker;
}

int main(int argc, const char * argv[])
{
    Mat frame;
    Mat frame_gray;
    Mat drawToFrame;
    
    Mat cameraMatrix = Mat::eye(3,3,CV_64F);
    Mat distanceCoefficients;
    vector<Mat> savedImages;
    vector<vector<Point2f>> markerCorners, rejectedCandidates;
    VideoCapture vid(0);
    if (!vid.isOpened())
    {
        return 0;
    }
    int framesPerSecond = 20;
    float num_frames = 15;
    double fps = 0, presicion = 0, presicionT;
    int counter = 0;
    namedWindow("Webcam", CV_WINDOW_AUTOSIZE);
    
    clock_t start, end;
    
    start = end = 0;
    
    while (true)
    {
        start = clock();

        if (!vid.read(frame))
            break;
        
        vector<Vec2f> foundpoints;
        
        bool found = false;
        
        threshold(frame, drawToFrame, 100, 255, THRESH_BINARY_INV);
        floodFill(drawToFrame, cv::Point(0,0), Scalar(255));
        bitwise_not(drawToFrame, drawToFrame);
        GaussianBlur( drawToFrame, drawToFrame, Size(9, 9), 2, 2 );
        cvtColor( drawToFrame, drawToFrame, COLOR_BGR2GRAY );
        Canny( drawToFrame, drawToFrame, 0, 0,
           3);
        found = findCirclesGrid(drawToFrame, chessboardDimension, foundpoints,  CALIB_CB_SYMMETRIC_GRID);
        drawChessboardCorners(drawToFrame, chessboardDimension, foundpoints, found);
        
        //cout << foundpoints.size() <<endl;
        
        //frame.copyTo(drawToFrame);

        imshow("Webcam", drawToFrame);
        
        char character = waitKey(1000/framesPerSecond);
        end = clock() - start;
        double seconds = ((float)end)/CLOCKS_PER_SEC;
        cout << "Time taken : " << seconds << " seconds" << endl;
        
        // Calculate frames per second
        fps  = num_frames / seconds;
        cout << "Estimated frames per second : " << fps << endl;
        start = end = 0;
        presicion += foundpoints.size() / (20.0);
        presicionT = presicion / counter;
        counter++;
        cout << "Relative Presicion : " << presicion <<endl;
        cout << "Total Presicion : " << presicionT <<endl;
    }
    return 0;
}
