
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <list>

using namespace std;
using namespace cv;

Mat SegmentAgave(Mat imagen, list <int> &lsegmentPixels);
Mat GroupAgave(Mat imagen, list <int> &lsegmentPixels, list <list <int> > &lgroupPixels);


RNG rng(12345);



int main(int argc, char *argv[])
{
    Mat imgIn   = imread("/home/cesar/AgaveDetection/Images/Agave2.jpg", CV_LOAD_IMAGE_COLOR);

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

    waitKey(0);

    return 0;
}

Mat SegmentAgave(Mat imgIn, list <int> &lsegmentPixels)
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
                     << "\tCol: " << lsegmentPixels.front()%imgOut.cols << endl;
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



Mat GroupAgave(Mat imagen, list <int> &lsegmentPixels, list <list <int> > &lgroupPixels)
{
   /* Mat imgOut = imagen.clone();
    Vec3b   vColor;

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( imagen, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    Mat drawing = Mat::zeros( imagen.size(), CV_8UC3 );

    for( size_t i = 0; i< contours.size(); i++ )
    {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
    }
*/
    return imagen;
}

/*

Mat GroupAgave(Mat imagen, list <int> &lsegmentPixels, list <list <int> > &lgroupPixels)
{
    Mat imgOut = imagen.clone();
    //Vec3b   vColor;

    int nPixelRow           = 0;
    int nPixelCol           = 0;
    int nActualPixel        = 0;
    int nNeighborPixel      = 0;
    int nListPixelPosition  = 1;
    bool bMorePixelInGroup  = true;
    list <int> lPixelsInGroup;
    list <int> lsegmentPixelsCopy;

    while(lsegmentPixels.size())
    {
        cout << "Position: " << lsegmentPixels.back() << endl;

        lPixelsInGroup.push_front(lsegmentPixels.back());

        bMorePixelInGroup  = true;

        while(bMorePixelInGroup)
        {
            nActualPixel    = lsegmentPixels.back();
            nPixelRow       = lsegmentPixels.back() / imgOut.cols;
            nPixelCol       = lsegmentPixels.back() % imgOut.cols;

            cout << "nActualPixel: " << nActualPixel << endl;
            cout << "nPixelRow: " << nPixelRow << endl;
            cout << "nPixelCol: " << nPixelCol << endl;
            cout << "x-1: " << ( nPixelRow * imgOut.cols ) + nPixelCol-1 << endl;
            cout << "x+1: " << ( nPixelRow * imgOut.cols ) + nPixelCol+1 << endl;
            cout << "y-1: " << ( (nPixelRow-1) * imgOut.cols ) + nPixelCol << endl;
            cout << "y+1: " << ( (nPixelRow+1) * imgOut.cols ) + nPixelCol << endl;

            // Add nActualPixel to the lPixelsInGroup list
            lPixelsInGroup.push_back(nActualPixel);

            // Remove last element of lsegmentPixels list
            lsegmentPixels.pop_back();

            if(nPixelCol-1 >= 0)
            {
                // Assign pixel number to nNeighborPixel
                nNeighborPixel = ( nPixelRow * imgOut.cols ) + nPixelCol-1;

                cout << "nPixelCol-1 >= 0 " << nNeighborPixel << endl;

                // Make a copy of lsegmentPixels list
                lsegmentPixelsCopy  = lsegmentPixels;
                nListPixelPosition  = 1;

                cout << "lPixelsInGroupCopy Size: " << lPixelsInGroupCopy.Size << endl;

                // Verify if nNeighborPixel is in lsegmentPixelsCopy
                while(lsegmentPixelsCopy.size())
                {
                    if(lsegmentPixelsCopy.back() == nNeighborPixel)
                    {
                        // Add nNeighborPixel to the lPixelsInGroup list
                        lPixelsInGroup.push_back(nNeighborPixel);

                        // Erase the element of nNeighborPixel from the lsegmentPixels list
                        lsegmentPixels.erase(nListPixelPosition);

                        // Clear lsegmentPixelsCopy list
                        lsegmentPixelsCopy.clear();
                    }
                    else
                    {
                        // Remocoutve last element of copy of lsegmentPixels list
                        lsegmentPixelsCopy.pop_back();
                        nListPixelPosition++;
                    }

                    // Remove last element of copy of lsegmentPixels list
                    lsegmentPixels.pop_back();
                }
            }

            if(nPixelCol+1 < imgOut.cols)
            {
                nNeighborPixel = ( nPixelRow * imgOut.cols ) + nPixelCol+1;
                cout << "nPixelCol+1 < imgOut.cols " << nNeighborPixel << endl;
            }
            if(nPixelRow-1 >= 0)
            {
                nNeighborPixel = ( (nPixelRow-1) * imgOut.cols ) + nPixelCol;
                cout << "nPixelRow-1 >= 0 " << nNeighborPixel << endl;
            }
            if(nPixelRow+1 < imgOut.rows)
            {
                nNeighborPixel = ( (nPixelRow+1) * imgOut.cols ) + nPixelCol;
                cout << "nPixelRow+1 < imgOut.rows " << nNeighborPixel << endl;
            }
        }

        //lgroupPixels.push_front(list <int> {10});

    }

    return imgOut;
}
*/
