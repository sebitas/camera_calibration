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
    RNG rng(12345);
    
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    while (true)
    {
        start = clock();
        
        if (!vid.read(frame))
            break;
        
        vector<Vec2f> foundpoints;
        
        //bool found = false;
        
        threshold(frame, drawToFrame, 144, 255, THRESH_BINARY_INV);
        floodFill(drawToFrame, cv::Point(0,0), Scalar(255));
        cvtColor( drawToFrame, drawToFrame, COLOR_BGR2GRAY );
        bitwise_not(drawToFrame, drawToFrame);
        //        GaussianBlur( drawToFrame, drawToFrame, Size(9, 9), 2, 2 );
        
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        
        findContours( drawToFrame, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
        vector<RotatedRect> minRect( contours.size() );
        vector<RotatedRect> minEllipse( contours.size() );
        vector<int> points;
        
        for( size_t i = 0; i < contours.size(); i++ )
        {
            minRect[i] = minAreaRect( Mat(contours[i]) );
            if(contours[i].size() > 5 )
            {
                minEllipse[i] = fitEllipse( Mat(contours[i]) );
            }
        }
        
        int jj = 0;
        for( size_t i = 0; i< contours.size(); i++ )
        {
            // contour
            drawContours( drawToFrame, contours, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            // ellipse
            if (2500 > minEllipse[i].size.width * minEllipse[i].size.height && minEllipse[i].size.width * minEllipse[i].size.height > 100)
            {
                jj++;
                points.push_back(i);
                ellipse( drawToFrame, minEllipse[i], color, 2, 8 );
                // rotated rectangle
                Point2f rect_points[4];
                minRect[i].points( rect_points );
                for( int j = 0; j < 4; j++ )
                    line( drawToFrame, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
            }
        }
        
        if(points.size() == 20)
        {
            for (int z = 1; z < points.size(); z++)
            {
                Point2f rect_points[4];
                Point2f rect_points_1[4];
                
                
                minRect[points[z-1]].points( rect_points );
                minRect[points[z]].points( rect_points_1 );
                //circle( drawToFrame, rect_points[0],5, color, 5);
                line(drawToFrame, rect_points[0], rect_points_1[1], color, 5);
            }
        }
        
        //found = findCirclesGrid(drawToFrame, chessboardDimension, foundpoints,  CALIB_CB_SYMMETRIC_GRID);
        //drawChessboardCorners(drawToFrame, chessboardDimension, foundpoints, found);
        
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
        presicion += contours.size() / (20.0);
        presicionT = presicion / counter;
        counter++;
        cout << "Relative Presicion : " << presicion <<endl;
        cout << "Total Presicion : " << jj <<endl;
    }
    return 0;
}
