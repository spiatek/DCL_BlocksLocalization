/*
 * ExtractColor.hpp
 *
 *  Created on: 16-01-2013
 *      Author: spiatek
 */

#ifndef EXTRACTCOLOR_PROCESSOR_HPP_
#define EXTRACTCOLOR_PROCESSOR_HPP_

#include <cv.h>
#include <highgui.h>

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Property.hpp"

namespace Processors {
namespace ExtractColor {

struct ExtractColor_Props: public Base::Props
{
	cv::Mat rgb_ranges;

	void load(const ptree & pt)
	{
		boost::numeric::ublas::matrix <double> rgbMatrixUblas = str2mat(pt.get <std::string> ("ranges"), 3, 2);
		rgb_ranges = cv::Mat(3, 2, CV_32F);
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 2; ++j) {
				rgb_ranges.at <float> (i, j) = rgbMatrixUblas(i, j);
			}
		}

		/*type = pt.get("type",0);
		threshold = pt.get("threshold",100);
		rho = pt.get("rho",1);
		theta = pt.get("theta",1);
		srn = pt.get("srn",0);
		stn = pt.get("stn",0);*/
	}

	void save(ptree & pt)
	{
		//pt.put("type", type);
	}

};

class ExtractColor_Processor: public Base::Component
{
public:
        ExtractColor_Processor(const std::string & name = "");
        virtual ~ExtractColor_Processor();

    	Base::Props * getProperties()
    	{
    		return &props;
    	}

protected:

        /*!
    	 * Method called when component is initialized
    	 * \return true on success
    	 */
        bool onInit();

    	/*!
    	 * Method called when component is finished
    	 * \return true on success
    	 */
        bool onFinish();

    	/*!
    	 * Method called when step is called
    	 * \return true on success
    	 */
        bool onStep();

        /*!
         * Method called when component is started
         * \return true on success
         */
        bool onStart();

    	/*!
    	 * Method called when component is stopped
    	 * \return true on success
    	 */
        bool onStop();

private:       

        void onNewImage();

		/** New image event handler. */
		Base::EventHandler <ExtractColor_Processor> h_onNewImage;
		
		/** Image stream. */
		Base::DataStreamIn <cv::Mat, Base::DataStreamBuffer::Newest> in_img;

		/** Position stream. */
		Base::DataStreamOut <cv::Mat> out_threshold;

        /** Raised when block has been located on the image. */
        Base::Event* newImage;

    	ExtractColor_Props props;

    	Base::Property<int> BFrom;
    	Base::Property<int> RFrom;
    	Base::Property<int> GFrom;

    	Base::Property<int> BTo;
    	Base::Property<int> RTo;
    	Base::Property<int> GTo;
};
}
}

/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("ExtractColor", Processors::ExtractColor::ExtractColor_Processor, Common::Panel_Empty)

#endif /* EXTRACTCOLOR_PROCESSOR_HPP_ */
