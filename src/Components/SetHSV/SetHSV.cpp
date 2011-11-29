/*!
 * \file
 * \brief
 */

#define UNLOADED 0
#define BLUE 1
#define	RED	2
#define GREEN 3
#define YELLOW 4

//TODO: create new DCL containing SetHSV and FindBlock

#include <memory>
#include <boost/thread.hpp>
#include "Common/xdr/xdr_iarchive.hpp"

#include "SetHSV.hpp"
//#include "Types/Mrrocpp_Proxy/BReading.hpp"
#include "Common/Logger.hpp"

using namespace boost;
using namespace cv;
using namespace std;

namespace Processors {
namespace SetHSV {

SetHSV_Processor::SetHSV_Processor(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello SetHSV_Processor\n";
}

SetHSV_Processor::~SetHSV_Processor()
{
	LOG(LTRACE) << "Good bye SetHSV_Processor\n";
}

bool SetHSV_Processor::onInit()
{
	LOG(LTRACE) << "SetHSV_Processor::initialize\n";

	h_onNewImage.setup(this, &SetHSV_Processor::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	h_onRpcCall.setup(this, &SetHSV_Processor::onRpcCall);
	registerHandler("onRpcCall", &h_onRpcCall);

	newImage = registerEvent("newImage");
	rpcResult = registerEvent("rpcResult");

	registerStream("in_img", &in_img);

	registerStream("in_rpc", &in_rpc);
	registerStream("out_rpc", &out_rpc);

	registerStream("out_hue", &out_hue);
	registerStream("out_saturation", &out_saturation);
	registerStream("out_value", &out_value);
	registerStream("out_threshold", &out_threshold);

	color = UNLOADED;
	counter = 0;

	return true;
}

bool SetHSV_Processor::onFinish()
{
	LOG(LTRACE) << "SetHSV_Processor::finish\n";
	return true;
}

bool SetHSV_Processor::onStep()
{
	LOG(LTRACE) << "SetHSV_Processor::step\n";
	return true;
}

bool SetHSV_Processor::onStop()
{
	return true;
}

bool SetHSV_Processor::onStart()
{
	return true;
}

void SetHSV_Processor::onNewImage()
{

	int do_reset = props.reset;
	int cmax = props.count_max;

	counter++;

	if(counter > cmax) {
		LOG(LNOTICE) << "Counter reset" << "\n";
		color = UNLOADED;
		counter = 0;
	}

	if(do_reset == 1 && color == UNLOADED) {
		//LOG(LERROR) << "onNewImage(): undefined color\n";
		return;
	}
	else {
		LOG(LNOTICE) << "onNewImage(): color " << color << "\n";
	}

	if(color == UNLOADED) {
		params = props.other_params;
	}

	try {

		cv::Mat hsv_img = in_img.read();
		cv::Size hsv_size = hsv_img.size();

		hue_img.create(hsv_size, CV_8UC1);
		saturation_img.create(hsv_size, CV_8UC1);
		value_img.create(hsv_size, CV_8UC1);
		threshold_img.create(hsv_size, CV_8UC1);

		if (hsv_img.isContinuous() && threshold_img.isContinuous() && value_img.isContinuous()
								   && hue_img.isContinuous() &&     saturation_img.isContinuous()) {
			hsv_size.width *= hsv_size.height;
			hsv_size.height = 1;
		}

		hsv_size.width *= 3;

		for (int i = 0; i < hsv_size.height; i++) {

			const uchar* hsv_p = hsv_img.ptr <uchar> (i);
			uchar* hue_p = hue_img.ptr <uchar> (i);
			uchar* sat_p = saturation_img.ptr <uchar> (i);
			uchar* val_p = value_img.ptr <uchar> (i);
			uchar* seg_p = threshold_img.ptr <uchar> (i);

			int j, k;
			for(j = 0, k = 0; j < hsv_size.width; j += 3, ++k)
			{
				hue_p[k] = hsv_p[j];
				sat_p[k] = hsv_p[j + 1];
				val_p[k] = hsv_p[j + 2];
				seg_p[k] = 0;

				if(int(hue_p[k]) >= int(params.at<float>(0,0)) && int(hue_p[k]) <= int(params.at<float>(0,1))) {
					if(int(sat_p[k]) >= int(params.at<float>(1,0)) && int(sat_p[k]) <= int(params.at<float>(1,1))) {
						if(int(val_p[k]) >= int(params.at<float>(2,0)) && int(val_p[k]) <= int(params.at<float>(2,1))) {
							seg_p[k] = 255;
						}
					}
				}
			}

		}

		out_hue.write(hue_img);
		out_saturation.write(saturation_img);
		out_value.write(value_img);
		out_threshold.write(threshold_img);

		LOG(LNOTICE) << "onNewImage(): end\n";

		newImage->raise();
	}
	catch (Common::DisCODeException& ex) {
		LOG(LERROR) << ex.what() << "\n";
		ex.printStackTrace();
		exit(EXIT_FAILURE);
	}
	catch (const char * ex) {
		LOG(LERROR) << ex;
	}
	catch (...) {
		LOG(LERROR) << "SetHSV_Processor::onNewImage failed\n";
	}
}

void SetHSV_Processor::onRpcCall()
{

	LOG(LNOTICE) << "onRpcCall start\n";

	try {

		//TODO: bug - onRpcCall doesn't work when color is set
		color = UNLOADED;
		counter = 0;

		Types::Mrrocpp_Proxy::BReading br;

		xdr_color = in_rpc.read();
		xdr_color >> color;

		LOG(LNOTICE) << "onRpcCall(): color parameter read: " << color << "\n";

		if(color == BLUE) {
			//TODO: blue block configuration modification - unable to locate block
			params = props.blue_params;
		}
		else if(color == RED) {
			params = props.red_params;
		}
		else if(color == GREEN) {
			params = props.green_params;
		}
		else if(color == YELLOW) {
			params = props.yellow_params;
		}
		else {
			LOG(LERROR) << "onRpcCall(): undefined color" << endl;
			return;
		}

		br.rpcReceived = true;
		out_rpc.write(br);

		LOG(LNOTICE) << "onRpcCall(): output message written\n";

		rpcResult->raise();

	}
	catch (Common::DisCODeException& ex) {
		LOG(LERROR) << ex.what() << "\n";
		ex.printStackTrace();
		exit(EXIT_FAILURE);
	}
	catch (const char * ex) {
		LOG(LERROR) << ex;
	}
	catch (...) {
		LOG(LERROR) << "SetHSV_Processor::onRpcCall failed\n";
	}

}

}//: namespace SetHSV
}//: namespace Processors
