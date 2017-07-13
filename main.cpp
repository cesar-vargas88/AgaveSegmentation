
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <list>

using namespace std;
using namespace cv;

Mat SegmentAgave(Mat imagen, list <int> lsegmentPixels);
Mat GroupAgave(Mat imagen, list <int> lsegmentPixels, list <list <int> > lgroupPixels);

int main(int argc, char *argv[])
{
    Mat imgIn   = imread("/home/cesar/AgaveDetection/Images/Agave1.jpg", CV_LOAD_IMAGE_COLOR);

    list <int>          lSegmentPixels;
    list <list <int> >  lGroupPixels;

    if(imgIn.empty())
    {
       cout << "Could not open image." << endl;
       return -1;
    }

    namedWindow("Input Image", CV_WINDOW_AUTOSIZE );
    imshow("Input Image", imgIn);

    namedWindow("Agave Segmentation", CV_WINDOW_AUTOSIZE );
    imshow("Agave Segmentation", SegmentAgave(imgIn, lSegmentPixels));

    namedWindow("Group Agave", CV_WINDOW_AUTOSIZE );
    imshow("Group Agave", GroupAgave(imgIn, lSegmentPixels, lGroupPixels));

    cout << "List Size: " << lSegmentPixels.size() << endl;

    waitKey(0);

    return 0;
}

Mat SegmentAgave(Mat imgIn, list <int> lsegmentPixels)
{
    Mat imgOut = imgIn.clone();
    Vec3b   vColor;

    for(int y=0; y < imgOut.rows; y++)
    {
        for(int x=0; x < imgOut.cols; x++)
        {
            // Get pixel
            vColor = imgOut.at<Vec3b>(Point(x, y));

            if((vColor[0] - vColor[2]) > 34)         // Difference between blue and red is mayor than 40
            {
                vColor[0] = 255;
                vColor[1] = 255;
                vColor[2] = 255;
                                        // ( ActualRow *  TotalCols  ) + ActualCol
                lsegmentPixels.push_front( (     y     * imgOut.cols ) +     x    );

                cout << "Num Pixel: " << lsegmentPixels.size()
                     << "\tSave Pixel: " << lsegmentPixels.front()
                     << "\tRow: " << lsegmentPixels.front()/imgOut.cols
                     << "\tCol: " << (lsegmentPixels.front()%imgOut.cols) << endl;
            }
            else
            {
                vColor[0] = 0;
                vColor[1] = 0;
                vColor[2] = 0;
            }

            // Set pixel
            imgOut.at<Vec3b>(Point(x, y)) = vColor;
        }
    }

    cout << "Rows: " << imgOut.rows << "\tCols: " << imgOut.cols << "\tTotal Pixels: " <<  imgOut.rows * imgOut.cols << endl;

    return imgOut;
}

Mat GroupAgave(Mat imagen, list <int> lsegmentPixels, list <list <int> > lgroupPixels)
{
    Mat imgOut = imagen.clone();
    //Vec3b   vColor;

    int nPixelRow = 0;
    int nPixelCol = 0;


    cout << "List Size: " << lsegmentPixels.size() << endl;
    /*
    for(int z=1 ; z<= lsegmentPixels.size() ; z++)
    {
        cout << lsegmentPixels.begin() << endl;
        lsegmentPixels.pop_back();


    }*/

    return imgOut;
}
