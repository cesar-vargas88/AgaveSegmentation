
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <list>

using namespace std;
using namespace cv;

Mat SegmentAgave(Mat imagen);
Mat LineDetection(Mat imagen);
Mat CircleDetection(Mat imagen);
Mat GroupAgave(Mat imagen);

int main(int argc, char *argv[])
{
    // Open the image
    Mat imgIn   = imread("/home/cesar/AgaveDetection/Images/CRT3_Arenal_V2 100%.jpg", CV_LOAD_IMAGE_COLOR);

    Mat imgLuv;
    Mat imgGray;
    Mat imgCrCb;
    Mat imgHSV;
    Mat imgHLS;
    Mat imgLab;

    list <int>          lSegmentPixels;
    list <list <int> >  lGroupPixels;

    // Verify that image opened correctly
    if(imgIn.empty())
    {
       cout << "Could not open image." << endl;
       return -1;
    }

    cvtColor(imgIn  ,    imgLuv  ,    CV_BGR2Luv);
    cvtColor(imgIn  ,    imgCrCb ,    CV_BGR2GRAY);
    cvtColor(imgIn  ,    imgCrCb ,    CV_BGR2YCrCb);
    cvtColor(imgIn  ,    imgHSV  ,    CV_BGR2HSV);
    cvtColor(imgIn  ,    imgHLS  ,    CV_BGR2HLS);
    cvtColor(imgIn  ,    imgLab  ,    CV_BGR2Lab);

    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/ColorLuv.jpg"    , imgLuv);
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/ColorCrCb.jpg"   , imgCrCb);
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/ColorHSV.jpg"    , imgHSV);
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/ColorHLS.jpg"    , imgHLS);
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/ColorLab.jpg"    , imgLab);

    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/AgaveSegmentation.jpg", SegmentAgave(imgIn));
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/AgaveSegmentation2.jpg", SegmentAgave(SegmentAgave(imgIn)));

    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/LineDetection.jpg", LineDetection(SegmentAgave(imgIn)));
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/CircleDetection.jpg", CircleDetection(SegmentAgave(imgIn)));

    // Create the image AgeveGroup.jpg
    imwrite("/home/cesar/AgaveDetection/Images/ProcessedImages/AgaveGroup.jpg", GroupAgave(SegmentAgave(imgIn)));

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

            if(((vColor[0] - vColor[2]) > 20) && ((vColor[0] - vColor[1]) > 2))         // Difference between blue and red is mayor than 38
            {
                /*vColor[0] = 255;
                vColor[1] = 255;
                vColor[2] = 255;*/
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

    cout << "Segment Agave" << endl;

    return imgOut;
}

Mat LineDetection(Mat src)
{
    Mat dst, cdst;
    Canny(src, dst, 50, 200, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }

    cout << "Line detection" << endl;

    return cdst;
}

Mat CircleDetection(Mat src)
{
    Mat src_gray;

    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

    vector<Vec3f> circles;

    HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/100, 200, 100, 0, 0 );

    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);

        // circle center
        circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
     }

    cout << "Circle detection" << endl;

    return src;
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
