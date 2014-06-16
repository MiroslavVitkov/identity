/*
 * main.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: Miroslav Vitkov
 */

#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]){
    cv::VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())      // check if we succeeded
        return -1;

    cv::Mat edges;
    cv::namedWindow("edges",1);
    for(;;)
    {
    	cv::Mat frame;
        cap >> frame; // get a new frame from camera
//        cv::imshow("edges", frame);

        cv::cvtColor(frame, edges, CV_BGR2GRAY);
        cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        cv::Canny(edges, edges, 0, 30, 3);
        cv::imshow("edges", edges);
        if(cv::waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

