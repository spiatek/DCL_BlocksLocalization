/*
 * CannyFilter.cpp
 *
 *  Created on: 14-01-2013
 *      Author: spiatek
 */

#include <memory>
#include <string>
#include <cmath>

#include <boost/bind.hpp>

#include "CannyFilter.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace CannyFilter {

using namespace Types;
using Types::Segmentation::SegmentedImage;

CannyFilter_Processor::CannyFilter_Processor(const std::string & name) :
        Base::Component(name),
        threshold1("threshold1", 70, "range"),
        threshold2("threshold2", 90, "range")
{
        LOG(LTRACE) << "Hello CannyFilter_Processor\n";
}

CannyFilter_Processor::~CannyFilter_Processor()
{
        LOG(LTRACE) << "Good bye CannyFilter_Processor\n";
}

void CannyFilter_Processor::prepareInterface()
{
    LOG(LTRACE) << "CannyFilter_Processor::prepareInterface\n";

    registerStream("in_img", &in_img);
    registerStream("out_edges", &out_edges);

    h_onNewImage.setup(boost::bind(&CannyFilter_Processor::onNewImage, this));
    registerHandler("onNewImage", &h_onNewImage);
    addDependency("onNewImage", &in_img);
}

bool CannyFilter_Processor::onInit()
{
        LOG(LTRACE) << "CannyFilter_Processor::initialize\n";

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

	cv::Canny(grayImage, edges, threshold1, threshold2, 3);

	out_edges.write(edges);
	//edgesFound->raise();
}
}

}
