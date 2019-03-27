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
	HWND hwndDesktop = GetDesktopWindow();

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwndDesktop, &windowsize);

	int srcheight = windowsize.bottom;
	int srcwidth = windowsize.right;

	int frames_per_second = 10;
	Size frame_size(srcwidth, srcheight);


	

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

	string window_name = "My Feed";
	namedWindow(window_name, WINDOW_NORMAL); //create a window called "My Camera Feed"
	Mat show = imread("guitar-05.jpg");
	imshow(window_name, show);

	
	int k = 0;
	while (true)
	{
		Mat dst;
		Mat src = hwnd2mat(hwndDesktop);	
		
		
		imwrite("temp.jpg", src);
		Mat LoadedImage = imread("temp.jpg");
		resize(LoadedImage, dst, frame_size);
		oVideoWriter.write(dst);

		if (remove("temp.jpg") != 0)
		{
			cout << "Error deleting file" << endl;
			break;
		}

		if (waitKey(10) == 27)
		{
			imwrite("temp.jpg", src);
			cout << "Video camera is disconnected" << endl;			
			break;
		}
		
	}

	//Flush and close the video file
	oVideoWriter.release();

	return 0;

}