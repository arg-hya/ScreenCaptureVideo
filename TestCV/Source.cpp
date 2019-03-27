#include "pch.h"


#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <iostream>

using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main(int argc, char **argv)
{
	//Open the default video camera
	String filename = "drop.avi";
	VideoCapture cap(filename.c_str());

	// if not success, exit program
	if (cap.isOpened() == false)
	{
		cout << "Cannot open the video camera" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	HWND hwndDesktop = GetDesktopWindow();
	
	//int key = 0;

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwndDesktop, &windowsize);

	int srcheight = windowsize.bottom;
	int srcwidth = windowsize.right;

	int frames_per_second = 10;
	Size frame_size(srcwidth, srcheight);

	int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH)); //get the width of frames of the video
	int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT)); //get the height of frames of the video

	//Size frame_size(frame_width, frame_height);

	//Create and initialize the VideoWriter object 
	VideoWriter oVideoWriter("MyVideo.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'),
		frames_per_second, frame_size, true);

	//If the VideoWriter object is not initialized successfully, exit the program
	if (oVideoWriter.isOpened() == false)
	{
		cout << "Cannot save the video to a file" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	vector<Mat> images;
	int k = 0;
	while (true)
	{
		

		
		
		//oVideoWriter.write(frame);

		Mat src = hwnd2mat(hwndDesktop);
		//cap.read(src);
		Mat dst;
		//Mat LoadedImage = imread("guitar-05.jpg");
		
		imwrite("temp.jpg", src);
		Mat LoadedImage = imread("temp.jpg");
		resize(LoadedImage, dst, frame_size);
		images.push_back(dst);
		if (remove("temp.jpg") != 0)
			cout << "Error deleting file" << endl;		
		else
			cout << "Done" << endl;
		//oVideoWriter.write(frame);

		// you can do some image processing here
		//imshow("output", images[k++]);

		if (getchar() == 'a')
		{
			cout << "Video camera is disconnected" << endl;

			for (int i = 0; i < images.size(); i++) 
			{
				oVideoWriter.write(images[i]);
			}

			//while (!images.empty())
			//{

			//	//imshow("output", images.back());
			//	oVideoWriter.write(images.back());
			//	//sum += images.back();
			//	images.pop_back();
			//}
			//cin.get(); //Wait for any key press
			break;
		}

		//if (getchar() == 'a')
		//{
			//namedWindow("output", WINDOW_NORMAL);
			//imwrite("Gray_Image.jpg", src);
			////write the video frame to the file
			//while (!images.empty())
			//{

			//	imshow("output", images.back());
			//	oVideoWriter.write(images.back());
			//	//sum += images.back();
			//	images.pop_back();
			//}
			//for (int i = 0; i < images.size(); i++) {
			//	oVideoWriter.write(images[i]);
			//}
			////oVideoWriter.write(src);
			//cout << "Esc key is pressed by the user. Stopping the video" << endl;
		//	break;
		//}
	}

	//Flush and close the video file
	oVideoWriter.release();

	return 0;

}