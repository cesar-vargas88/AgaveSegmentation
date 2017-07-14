
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <list>

using namespace std;
using namespace cv;

Mat SegmentAgave(Mat imagen);
Mat GroupAgave(Mat imagen);

int main(int argc, char *argv[])
{
    // Open the image
    Mat imgIn   = imread("/home/cesar/AgaveDetection/Images/CRT3_Arenal_V2 100%.jpg", CV_LOAD_IMAGE_COLOR);
    Mat imgOut = imgIn.clone();

    list <int>          lSegmentPixels;
    list <list <int> >  lGroupPixels;

    // Verify that image opened correctly
    if(imgIn.empty())
    {
       cout << "Could not open image." << endl;
       return -1;
    }

    //GaussianBlur( imgIn, imgOut, Size( 9, 9 ), 0, 0 );

    // Reduce noise with a kernel 3x3
    //blur( src_gray, detected_edges, Size(3,3) );

    // Canny detector
    //Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

    // Using Canny's output as a mask, we display our result
    //dst = Scalar::all(0);



    Mat gray, edge, draw;

    //cvtColor(imgIn, gray, CV_BGR2GRAY);ŝŝ

    //Canny(imgOut, edge, 50, 150, 3);
    //edge.convertTo(draw, CV_8U);
    // Create the image GaussianBlur

    // Create the image AgeveSegmentation.jpg
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/AgaveSegmentation.jpg", SegmentAgave(imgIn));

    // Create the image AgeveGroup.jpg
    //imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/AgaveGroup.jpg", GroupAgave(SegmentAgave(imgIn)));

    waitKey(0);

    return 0;
}

Mat SegmentAgave(Mat imgIn)
{
    Mat imgOut = imgIn.clone();
    Vec3b   vColor;

    for(int y=0; y < imgOut.rows; y++)
    {
        for(int x=0; x < imgOut.cols; x++)
        {
            // Get pixel
            vColor = imgOut.at<Vec3b>(Point(x, y));

            if(((vColor[0] - vColor[2]) > 25) && ((vColor[0] - vColor[1]) > 0))         // Difference between blue and red is mayor than 38
            {
                vColor[0] = 255;
                vColor[1] = 255;
                vColor[2] = 255;
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

    GaussianBlur( imgOut, imgIn, Size( 3, 3 ), 0, 0 );

    cout << "Rows: " << imgOut.rows << "\tCols: " << imgOut.cols << "\tTotal Pixels: " <<  imgOut.rows * imgOut.cols << endl;

    return imgOut;
}


Mat GroupAgave(Mat imgIn)
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;


    int thresh = 100;
    RNG rng(12345);

    // Detect edges using canny
    Canny( imgIn, canny_output, thresh, thresh*2, 3 );

    // Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<Moments> mu(contours.size());
    vector<Point2f> mc( contours.size());
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    int nCountContours = 1;

    cout << "\n\nThere are a total of " << contours.size() << " contours." << endl;
    cout << "\nInfo: Area and Contour Length" << endl;

    /// Calculate the area with the moments 00 and compare with the result of the OpenCV function
    for( int i = 0; i< contours.size(); i++ )
    {
        // Get the moments
        mu[i] = moments( contours[i], false );

        // Get the mass centers
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

        // Draw contours
        Scalar color = Scalar( 0, 0, 255 );
        //drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );

        if(contourArea(contours[i]) > -10)
        {
            // Draw circles
            color = Scalar( 0, 255, 0 );
            circle( drawing, mc[i], 1, color, 1, 4, 0 );

            cout << nCountContours++ << "-"<< i+1 << "\t  Area (M_00) = " << mu[i].m00 << "\t  Area OpenCV: " << contourArea(contours[i]) << "\t  Length: " << arcLength( contours[i], true ) << endl;
        }
    }

    return drawing;
}
