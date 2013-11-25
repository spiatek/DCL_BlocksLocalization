/*
 * CannyFilter.hpp
 *
 *  Created on: 14-01-2012
 *      Author: spiatek
 */

#ifndef CANNYFILTER_PROCESSOR_HPP_
#define CANNYFILTER_PROCESSOR_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Property.hpp"
#include "EventHandler2.hpp"
//#include "Props.hpp"

#include "Types/SegmentedImage.hpp"
#include "Types/DrawableContainer.hpp"
#include "Types/Drawable.hpp"
#include "Types/ImagePosition.hpp"

namespace Processors {
namespace CannyFilter {

//struct CannyFilter_Props: public Base::Props
//{
//
//	int threshold1, threshold2;
//
//	void load(const ptree & pt)
//	{
//		threshold1 = pt.get("threshold1",70);
//		threshold2 = pt.get("threshold2",90);
//	}
//
//	void save(ptree & pt)
//	{
//		pt.put("threshold1", threshold1);
//		pt.put("threshold2", threshold2);
//	}
//
//};

class CannyFilter_Processor: public Base::Component
{
public:
        CannyFilter_Processor(const std::string & name = "");
        virtual ~CannyFilter_Processor();
        void prepareInterface();

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

		/** Image stream. */
		Base::DataStreamIn <cv::Mat> in_img;

		/** Position stream. */
		Base::DataStreamOut <cv::Mat> out_edges;

		/** New image event handler. */
		Base::EventHandler2 h_onNewImage;

        /** Raised when block has been located on the image. */
        //Base::Event* edgesFound;

    	//CannyFilter_Props props;
        Base::Property<int> threshold1;
        Base::Property<int> threshold2;
};
}
}

/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("CannyFilter", Processors::CannyFilter::CannyFilter_Processor, Common::Panel_Empty)

#endif /* CANNYFILTER_PROCESSOR_HPP_ */
