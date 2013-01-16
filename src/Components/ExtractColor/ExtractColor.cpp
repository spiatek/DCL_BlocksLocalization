/*
 * ExtractColor.cpp
 *
 *  Created on: 16-01-2013
 *      Author: spiatek
 */

#include <memory>
#include <string>
#include <cmath>

#include "ExtractColor.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace ExtractColor {

using namespace Types;
using Types::Segmentation::SegmentedImage;

ExtractColor_Processor::ExtractColor_Processor(const std::string & name) :
        Base::Component(name)
{
        LOG(LTRACE) << "Hello ExtractColor_Processor\n";
}

ExtractColor_Processor::~ExtractColor_Processor()
{
        LOG(LTRACE) << "Good bye ExtractColor_Processor\n";
}

bool ExtractColor_Processor::onInit()
{
        LOG(LTRACE) << "ExtractColor_Processor::initialize\n";

        h_onNewImage.setup(this, &ExtractColor_Processor::onNewImage);
        registerHandler("onNewImage", &h_onNewImage);

        registerStream("in_img", &in_img);

        registerStream("out_lines", &out_lines);
        registerStream("out_linesVector", &out_linesVector);

        linesFound = registerEvent("linesFound");

        return true;
}

bool ExtractColor_Processor::onFinish()
{
        LOG(LTRACE) << "ExtractColor_Processor::finish\n";

        return true;
}

bool ExtractColor_Processor::onStep()
{
        LOG(LTRACE) << "ExtractColor_Processor::step\n";
        return true;
}

bool ExtractColor_Processor::onStop()
{
        return true;
}

bool ExtractColor_Processor::onStart()
{
        return true;
}

void ExtractColor_Processor::onNewImage()
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
