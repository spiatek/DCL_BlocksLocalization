/*
 * CannyFilter.cpp
 *
 *  Created on: 14-01-2013
 *      Author: spiatek
 */

#include <memory>
#include <string>
#include <cmath>

#include "CannyFilter.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace CannyFilter {

using namespace Types;
using Types::Segmentation::SegmentedImage;

CannyFilter_Processor::CannyFilter_Processor(const std::string & name) :
        Base::Component(name)
{
        LOG(LTRACE) << "Hello CannyFilter_Processor\n";
}

CannyFilter_Processor::~CannyFilter_Processor()
{
        LOG(LTRACE) << "Good bye CannyFilter_Processor\n";
}

bool CannyFilter_Processor::onInit()
{
        LOG(LTRACE) << "CannyFilter_Processor::initialize\n";

        h_onNewImage.setup(this, &CannyFilter_Processor::onNewImage);
        registerHandler("onNewImage", &h_onNewImage);

        registerStream("in_img", &in_img);

        registerStream("out_edges", &out_edges);

        edgesFound = registerEvent("edgesFound");

        return true;
}

bool CannyFilter_Processor::onFinish()
{
        LOG(LTRACE) << "CannyFilter_Processor::finish\n";

        return true;
}

bool CannyFilter_Processor::onStep()
{
        LOG(LTRACE) << "CannyFilter_Processor::step\n";
        return true;
}

bool CannyFilter_Processor::onStop()
{
        return true;
}

bool CannyFilter_Processor::onStart()
{
        return true;
}

void CannyFilter_Processor::onNewImage()
{
	cv::Mat image, grayImage, edges;
	image = in_img.read();
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);

	cv::Canny(grayImage, edges, props.threshold1, props.threshold2);

	out_edges.write(edges);
	edgesFound->raise();
}
}

}
