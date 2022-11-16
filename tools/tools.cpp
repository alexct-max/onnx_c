#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>
#include "cstdlib"
#include "cstdio"
#include <libgen.h>

using namespace std;
using namespace cv;

void sliceImg(string imgPath, int sliceHeight, int sliceWidth, float overlap);


int main(void)
{
    sliceImg("/media/alex/data/ro_img/1.JPG", 1500, 1500, 0.10);
    return 0;
}


void sliceImg(string imgPath, int sliceHeight, int sliceWidth, float overlap)
{   
    string outname_base = imgPath;
	char *p = (char *)outname_base.c_str();
	string outname = basename(p); //文件名
	string outdir = dirname(p);   //目录
    
    Mat img0 = imread(imgPath);


	int win_h = img0.rows;
	int win_w = img0.cols;

    int dx = int((1.-overlap)*sliceWidth);
    int dy = int((1.-overlap)*sliceHeight);

    cout << "dx: " << dx << endl;
    cout << "dy: " << dy << endl;
    cout << "win_h: " << win_h << endl;

    for(int i=0; i*dy<win_h; i++)
    {       // i++ i*dy
        for(int j=0; j*dy<win_w; j++)
        {  
            if(i+sliceHeight>win_h)
            {
                i = win_h-sliceHeight;
            // break;
            }
            if(j+sliceWidth>win_w)
            {
                j = win_w-sliceWidth;
                // break;
            }
            cout << "i: " << i << ", j: " << j << endl;
        }
    }
}