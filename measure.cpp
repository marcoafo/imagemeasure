#include <cstdio>
#include <vector>
#include <fstream>

#include <opencv2/opencv.hpp>

int threshold;
cv::Mat original;

double euclideanDist(cv::Point2f& p, cv::Point2f& q)
{
    cv::Point2f diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

struct contour_sorter // 'less' for contours
{
    bool operator ()( const std::vector<cv::Point>& a, const std::vector<cv::Point> & b )
    {
        cv::Rect ra(cv::boundingRect(a));
        cv::Rect rb(cv::boundingRect(b));
        return( ra.x < rb.x );
        // scale factor for y should be larger than img.width
        //return ( (ra.x + 1000*ra.y) < (rb.x + 1000*rb.y) );
    }
};


void ontrackbar_callback(int, void*)
{
	cv::Mat ranged;
	cv::cvtColor(original,ranged,CV_BGR2GRAY);
	cv::threshold(ranged,ranged,threshold,255,cv::THRESH_BINARY_INV);
	//cv::imshow("ranged",ranged);
	
	cv::Mat display = original.clone();
	
	cv::Mat spectrum = ranged.clone();
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours( spectrum, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

	std::sort(contours.begin(), contours.end(), contour_sorter());

	
	int count = 1;
	double ratio = 1;
	
	for(unsigned int j = 0; j < contours.size(); j++)
	{
		double a = cv::contourArea(contours[j]);
		if(a < 100 || a > 450000) continue;
		//std::printf("%f\n",a);
		cv::drawContours(display, contours, j , cv::Scalar(0,0,255), 2, 8 );
		
		cv::RotatedRect rect = cv::minAreaRect(contours[j]);
		cv::Point2f rect_points[4]; rect.points( rect_points );
		for( int k = 0; k < 4; k++ )
		{
			cv::line( display, rect_points[k], rect_points[(k+1)%4], cv::Scalar(0,255,0), 2, 8 );
		}
		
		char buf[10];
		std::sprintf(buf,"%d",count);
		cv::putText(display,buf,rect_points[3],cv::FONT_HERSHEY_SIMPLEX,0.6,cv::Scalar(255,0,255),1);
		
		if(count == 1) // coin
		{
			double distA = euclideanDist(rect_points[0],rect_points[1]);
			double distB = euclideanDist(rect_points[1],rect_points[2]);
			
			ratio = ((distA+distB)/2) / 0.955;
			
			std::printf("ratio: %f\n\n",ratio);
		
		}
		
		else
		{
			double distA = euclideanDist(rect_points[0],rect_points[1]);
			double distB = euclideanDist(rect_points[1],rect_points[2]);
			
			std::printf("distances for %d = %f in x %f in\n",count, distA / ratio, distB / ratio);
		}
		
		
		count++;

	}
	
	cv::imshow("display",display);
}

int main()
{
	threshold = 73;
	original = cv::imread("measure.png",CV_LOAD_IMAGE_ANYCOLOR);
	
	ontrackbar_callback(1,NULL);
	cv::namedWindow("measure", 1);
	cv::createTrackbar("threshold", "measure", &threshold, 255, ontrackbar_callback);

	while(true)
	{
		cv::waitKey(100);
	}
	
	return 0;

}




