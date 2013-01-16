/*
 * ExtractColor.hpp
 *
 *  Created on: 16-01-2012
 *      Author: spiatek
 */

#ifndef EXTRACTCOLOR_PROCESSOR_HPP_
#define EXTRACTCOLOR_PROCESSOR_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"

#include "Types/SegmentedImage.hpp"
#include "Types/DrawableContainer.hpp"
#include "Types/Drawable.hpp"
#include "Types/ImagePosition.hpp"

namespace Processors {
namespace ExtractColor {

struct ExtractColor_Props: public Base::Props
{
	int type, threshold;
	double rho, theta, srn, stn;

	void load(const ptree & pt)
	{
		type = pt.get("type",0);
		threshold = pt.get("threshold",100);
		rho = pt.get("rho",1);
		theta = pt.get("theta",1);
		srn = pt.get("srn",0);
		stn = pt.get("stn",0);
	}

	void save(ptree & pt)
	{
		pt.put("type", type);
		pt.put("threshold", threshold);
		pt.put("rho", rho);
		pt.put("theta", theta);
		pt.put("srn", srn);
		pt.put("stn", stn);
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
		Base::DataStreamIn <cv::Mat> in_img;

		/** Position stream. */
		Base::DataStreamOut <cv::Mat> out_lines;
		Base::DataStreamOut < vector<cv::Vec4i> > out_linesVector;

        /** Raised when block has been located on the image. */
        Base::Event* linesFound;

    	ExtractColor_Props props;
};
}
}

/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("ExtractColor", Processors::ExtractColor::ExtractColor_Processor, Common::Panel_Empty)

#endif /* EXTRACTCOLOR_PROCESSOR_HPP_ */
