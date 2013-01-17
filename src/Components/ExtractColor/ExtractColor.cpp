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
        registerStream("out_threshold", &out_threshold);

        newImage = registerEvent("newImage");

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
	cv::Mat image, thresholdedImage;
	cv::Mat rImg, gImg, bImg;

	image = in_img.read();
	cv::Size img_size = image.size();

	cv::Mat ranges = props.rgb_ranges;

	rImg.create(img_size, CV_8UC1);
	gImg.create(img_size, CV_8UC1);
	bImg.create(img_size, CV_8UC1);
	thresholdedImage.create(img_size, CV_8UC1);

	if (image.isContinuous() && rImg.isContinuous() && gImg.isContinuous()
			&& bImg.isContinuous() && thresholdedImage.isContinuous()) {
		img_size.width *= img_size.height;
		img_size.height = 1;
	}

	img_size.width *= 3;

	for(int i = 0; i < img_size.height; i++) {

		const uchar* orgp = image.ptr <uchar> (i);
		uchar* rp = rImg.ptr <uchar> (i);
		uchar* gp = gImg.ptr <uchar> (i);
		uchar* bp = bImg.ptr <uchar> (i);
		uchar* tp = thresholdedImage.ptr <uchar> (i);

		int j, k;
		for(j = 0, k = 0; j < img_size.width; j += 3, ++k)
		{
			rp[k] = orgp[j];
			gp[k] = orgp[j + 1];
			bp[k] = orgp[j + 2];
			tp[k] = 0;

			if(int(rp[k]) >= int(ranges.at<float>(0,0)) && int(rp[k]) <= int(ranges.at<float>(0,1))) {
				if(int(gp[k]) >= int(ranges.at<float>(1,0)) && int(gp[k]) <= int(ranges.at<float>(1,1))) {
					if(int(bp[k]) >= int(ranges.at<float>(2,0)) && int(bp[k]) <= int(ranges.at<float>(2,1))) {
						tp[k] = 255;
					}
				}
			}
		}

	}

	out_threshold.write(thresholdedImage);
	newImage->raise();
}
}

}
