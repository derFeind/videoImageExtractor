//============================================================================
// Name        : videoImageExtractor.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace std;

Mat frame;
int frameCount = 0;
vector<String> filenames;
vector<Mat> images;
vector<Mat> images_dest;
String folder = "/home/usr/Bilder/stitched big batch/middleRotation_chopped";

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		char file_name[100];
		sprintf(file_name, "image%d.jpg", frameCount + 1);
		bool suc = imwrite(file_name, frame);
		cout << "save curr image: " << file_name << endl;
		frameCount++;
	}
}

static void loadFiles() {
	glob(folder, filenames);
	for (int i = 0; i < filenames.size(); ++i) {
		images.push_back(imread(filenames.at(i)));
		cout << filenames.at(i) << " loaded" << endl;
	}
}

static int converImagesToDist() {

	FileStorage fs("./calibration.dat", FileStorage::READ);
	if (!fs.isOpened()) {
		std::cout << "filename of calibration file must be calibration.dat"
				<< std::endl;
		return EXIT_FAILURE;
	}

	Mat cameraMatrix, distortionCoefficients;
	//load data from file
	fs["Camera_Matrix"] >> cameraMatrix;
	fs["Distortion_Coefficients"] >> distortionCoefficients;

	for (int i = 0; i < images.size(); i++) {
		String filename_dst = filenames.at(i) + "_dst";
		cout << "check if DST file allready exist" << endl;
		if (!imread(filename_dst).data) {
			cout << "file does not exist creaing DST file" << endl;
			//load image
			Mat img = images.at(i);
			Mat imgU;
			//TODO: undistort mit map, die einmal im Konstruktor generiert wird
			undistort(img, imgU, cameraMatrix, distortionCoefficients);
			imwrite(filename_dst, imgU);
			cout << filename_dst << " written " << endl;
		} else {
			cout << "file: " << filename_dst << " allready exists" << endl;
		}

	}

	return EXIT_SUCCESS;
}

static void chopImages() {
	cout << "chopping images" << endl;
	for (int i = 0; i < images.size(); ++i) {
		cout << "chopping image: " << filenames.at(i) << endl;
		Mat tmp, src;
		src = images.at(i);
		tmp = src(Rect(560, 240, 800, 600));
		String filename_choppedImage = filenames.at(i) + "_chopped";
		cout << "writing chopped image to: " << filename_choppedImage << endl;
		imwrite(filename_choppedImage, tmp);
		src.deallocate();
		tmp.deallocate();
	}
}

int main() {

	//loadFiles();
	//converImagesToDist();

	//chopImages();
	/*

	 VideoCapture cap("./GOPR0598.MP4"); // open the default camera
	 //Video Capture cap(path_to_video); // open the video file
	 if(!cap.isOpened())  // check if we succeeded
	 return -1;

	 namedWindow("Video",1);
	 setMouseCallback("Video", CallBackFunc, NULL);
	 for(;;)
	 {
	 cap >> frame; // get a new frame from camera
	 imshow("Video", frame);
	 if(waitKey(30) >= 0) break;
	 }
	 */
	// the camera will be deinitialized automatically in VideoCapture destructor
	// sync video's
	String video_1 = "/home/usr/Videos/Record_2016-02-09_11-50-39.ravi";
	String video_2 = "/home/usr/Videos/GOPR0684.MP4";
	VideoCapture capture_1;
	capture_1.open(video_1);
	VideoCapture capture_2;
	capture_2.open(video_2);

	if (!capture_1.isOpened() || !capture_2.isOpened()) {
		std::cout << "Error opening video" << std::endl;
		std::cout.flush();
		exit(0);
	}
	std::cout << "Capture is opened" << std::endl;
	int frameCount_1 = capture_1.get(CV_CAP_PROP_FRAME_COUNT);
	int frameCount_2 = capture_2.get(CV_CAP_PROP_FRAME_COUNT);
	double fps_1 = capture_1.get(CV_CAP_PROP_FPS);
	double fps_2 = capture_2.get(CV_CAP_PROP_FPS);

	cout << "video 1 stats: " << endl;
	std::cout << "Video with " << frameCount_1 << " frames with: " << fps_1
			<< " FPS" << std::endl;
	cout << "lenght of the video: " << frameCount_1 / fps_1 << " seconds"
			<< endl;
	cout << "video = (" << capture_1.get(CV_CAP_PROP_FRAME_WIDTH) << "x"
			<< capture_1.get(CV_CAP_PROP_FRAME_HEIGHT) << ")" << endl;
	cout << "####################" << endl;

	cout << "video 2 stats: " << endl;
	std::cout << "Video with " << frameCount_2 << " frames with: " << fps_2
			<< " FPS" << std::endl;
	cout << "lenght of the video: " << frameCount_2 / fps_2 << " seconds"
			<< endl;
	cout << "video = (" << capture_2.get(CV_CAP_PROP_FRAME_WIDTH) << "x"
			<< capture_2.get(CV_CAP_PROP_FRAME_HEIGHT) << ")" << endl;
	cout << "####################" << endl;
	//namedWindow("video", CV_WINDOW_AUTOSIZE);

	Mat currImg_1, currImg_2;
	char key = 0;

	struct stat attr_1, attr_2;

	//show video
	//set up time
	stat("/home/usr/Videos/Record_2016-02-09_11-50-39.ravi", &attr_1);
	stat("/home/usr/Videos/GOPR0684.MP4", &attr_2);
	time_t lastMod = attr_1.st_mtime;
	printf("file was last modified: %s", ctime(&lastMod));

	long double timeUMS = lastMod * 1000;
	//cout << lastMod << "|" << lastMod*1000 << "|" << 1000/fps << endl;

	int currentFrameVideo_1 = 0;
	int currentFrameVideo_2 = 0;


	//wrapper(wrapper_width,&wrapper_height,CV_8UC1, Scalar(0));

	capture_1.read(currImg_1);
	capture_2.read(currImg_2);
	Size sizeImg_1 = currImg_1.size();
	Size sizeImg_2 = currImg_2.size();

	int wrapper_height;
	if (sizeImg_1.height < sizeImg_2.height) {
		wrapper_height = sizeImg_2.height;
	} else {
		wrapper_height = sizeImg_1.height;
	}

	bool runVideo_1 = true, runVideo_2 = true;
	int difference = 46;
	while (key != 'q') {
		char str[200];
		//sprintf(str, "frame: (%d / %d)", currentFrame, frameCount);
		if(difference > 0){
			capture_2.read(currImg_2);
			currentFrameVideo_2++;
			difference--;
		}else{
			if (runVideo_1) {
				capture_1.read(currImg_1);
				currentFrameVideo_1++;
			}
			if (runVideo_2) {
				capture_2.read(currImg_2);
				currentFrameVideo_2++;
			}
		}


		Size sizeImg_1 = currImg_1.size();
		Size sizeImg_2 = currImg_2.size();
		Mat wrapper(wrapper_height, sizeImg_1.width + sizeImg_2.width, CV_8UC3);
		Mat left(wrapper, Rect(0, 0, sizeImg_1.width, sizeImg_1.height));
		currImg_1.copyTo(left);
		Mat right(wrapper,
				Rect(sizeImg_1.width, 0, sizeImg_2.width, sizeImg_2.height));
		currImg_2.copyTo(right);
		imshow("video", wrapper);

		// resize(currImg,resImg,Size(),0.5,0.5);
		// putText(resImg,str,Point2f(100,100),FONT_HERSHEY_PLAIN,2,Scalar(0,0,255,255));
		// putText(resImg,ctime(&lastMod),Point2f(100,150),FONT_HERSHEY_PLAIN,2,Scalar(0,0,255,255));


		key = waitKey(1000 / 29.97); // toDo Frames immer FPS immer 30
		if (key == '1') {
			runVideo_1 = false;
		}
		if (key == 'a'){
			runVideo_1 = true;
		}
		if (key == '2') {
			runVideo_2 = false;
		}
		if (key == 's'){
			runVideo_2 = true;
		}
		if (key == 'k'){
			currentFrameVideo_2++;
		}

		cout << "video 1: (" << currentFrameVideo_1 << "/" << frameCount_1 << ") Video 2: (" << currentFrameVideo_2 << "/" << frameCount_2 << ")" << endl;

		timeUMS = timeUMS + (1000.0 / 30);
		lastMod = (timeUMS / 1000.0);
	}

	cout << "difference: " << currentFrameVideo_1-currentFrameVideo_2 << "|" << currentFrameVideo_2-currentFrameVideo_1 << endl;

	capture_1.release();
	capture_2.release();

	//#####  best extracter / viewer ######
	/*
	 loadFiles();

	 cout << "loading done" << endl;
	 namedWindow("output", CV_WINDOW_AUTOSIZE);
	 char key = 0;

	 Mat currImg,resImg;
	 int i = 0;
	 int k = 0;
	 int out = 0;
	 while(key != 'q'){
	 currImg = images.at(i);
	 resize(currImg,resImg,Size(),0.5,0.5);
	 imshow("output",resImg);
	 k = waitKey(0);
	 if(k==1048689){
	 break;
	 }
	 if(k==1113939){
	 i++;
	 }
	 if(k==1048691){
	 //todo Save image
	 imwrite(filenames.at(i)+"_good",currImg);
	 cout << "save image" << endl;
	 }
	 }

	 destroyAllWindows();
	 */
	cout << "###done###" << endl;
	waitKey(0);
	return 0;

}
