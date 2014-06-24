/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

 /** Global variables */
String face_cascade_name = "haarcascades/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
const string pictures_dir = "./pictures";
RNG rng(12345);

bool low_pass_filter( std::vector<Mat> &detected_faces );
std::vector<cv::Mat> detect_faces( Mat frame );

int main(int argc, const char *argv[]) {
    // Check for valid command line arguments, print usage
    // if no arguments were given.
    if (argc != 2) {
        cout << "usage: " << argv[0] << " <trained_model>" << endl;
        exit(1);
    }
    // The following lines create an LBPH model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    //
    // The LBPHFaceRecognizer uses Extended Local Binary Patterns
    // (it's probably configurable with other operators at a later
    // point), and has the following default values
    //
    //      radius = 1
    //      neighbors = 8
    //      grid_x = 8
    //      grid_y = 8
    //
    // So if you want a LBPH FaceRecognizer using a radius of
    // 2 and 16 neighbors, call the factory method with:
    //
    //      cv::createLBPHFaceRecognizer(2, 16);
    //
    // And if you want a threshold (e.g. 123.0) call it with its default values:
    //
    //      cv::createLBPHFaceRecognizer(1,8,8,8,123.0)
    //
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->load(argv[1]);

    // Open the default camera.
    cv::VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())      // check if we succeeded
        return -1;

    Mat frame;

    while(1)
    {
        cap >> frame;
        std::vector<Mat> detected_faces =  detect_faces( frame );
        if( low_pass_filter(detected_faces) )
        {
            int predictedLabel = model->predict(detected_faces[0]);  // TODO: just a mock, fix for n targets.
            //
            // To get the confidence of a prediction call the model with:
            //
            //      int predictedLabel = -1;
            //      double confidence = 0.0;
            //      model->predict(testSample, predictedLabel, confidence);
            //
            string result_message = format("Predicted class = %d .", predictedLabel);
            cout << result_message << endl;
        }

    }
    return 0;
}

bool low_pass_filter(std::vector<Mat> &detected_faces)
{
    static unsigned consecutive_positives = 0;
    unsigned current_faces = detected_faces.size();
    if( current_faces )
    {
        ++consecutive_positives;
    } else {
        consecutive_positives = 0;
    }

    if( current_faces >= 3 )
    {
        return true;
    } else {
        return false;
    }
}

std::vector<cv::Mat> detect_faces( Mat frame )
{
    std::vector<Rect> faces_rects;
    Mat frame_gray;
    std::vector<Mat> faces_images;

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces_rects, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    // Crop the faces from the picture and return them in a vector.
      for(size_t i = 0; i < faces_rects.size(); ++i)
      {
          Rect myROI(faces_rects[i].x, faces_rects[i].y, faces_rects[i].width, faces_rects[i].height);
          Mat cropped = frame(myROI);
          faces_images.push_back( cropped );
      }  // for each face in image

    return faces_images;
}
