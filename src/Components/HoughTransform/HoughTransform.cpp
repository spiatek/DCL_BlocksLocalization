/*
 * HoughTransform.cpp
 *
 *  Created on: 14-01-2013
 *      Author: spiatek
 */

#include <memory>
#include <string>
#include <cmath>

#include "HoughTransform.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace HoughTransform {

using namespace Types;
using Types::Segmentation::SegmentedImage;

HoughTransform_Processor::HoughTransform_Processor(const std::string & name) :
        Base::Component(name)
{
        LOG(LTRACE) << "Hello HoughTransform_Processor\n";
}

HoughTransform_Processor::~HoughTransform_Processor()
{
        LOG(LTRACE) << "Good bye HoughTransform_Processor\n";
}

bool HoughTransform_Processor::onInit()
{
        LOG(LTRACE) << "HoughTransform_Processor::initialize\n";

        h_onNewImage.setup(this, &HoughTransform_Processor::onNewImage);
        registerHandler("onNewImage", &h_onNewImage);

        registerStream("in_img", &in_img);

        registerStream("out_lines", &out_lines);
        registerStream("out_linesVector", &out_linesVector);

        linesFound = registerEvent("linesFound");

        return true;
}

bool HoughTransform_Processor::onFinish()
{
        LOG(LTRACE) << "HoughTransform_Processor::finish\n";

        return true;
}

bool HoughTransform_Processor::onStep()
{
        LOG(LTRACE) << "HoughTransform_Processor::step\n";
        return true;
}

bool HoughTransform_Processor::onStop()
{
        return true;
}

bool HoughTransform_Processor::onStart()
{
        return true;
}

void HoughTransform_Processor::onNewImage()
{
	cv::Mat image, bgrImage;
	image = in_img.read();

	cvtColor(image, bgrImage, CV_GRAY2BGR);

	if(props.type == 0) {
		vector<cv::Vec2f> lines;
		HoughLines(image, lines, props.rho, props.theta*CV_PI/180, props.threshold, props.srn, props.stn);

		LOG(LTRACE) << "Number of lines " << lines.size() << "\n";

		for(size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*(a));
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*(a));
			line(bgrImage, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
		}
	}
	else if(props.type == 1) {
		vector<cv::Vec4i> lines;
		HoughLinesP(image, lines, props.rho, props.theta*CV_PI/180, props.threshold, props.srn, props.stn );
		for( size_t i = 0; i < lines.size(); i++ )
		{
			cv::Vec4i l = lines[i];
			line( bgrImage, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
		}
		out_linesVector.write(lines);
	}

	out_lines.write(bgrImage);
	linesFound->raise();
}
}

}
