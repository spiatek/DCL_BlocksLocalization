/*
 * FindBlock.hpp
 *
 *  Created on: 22-10-2011
 *      Author: spiatek
 */

#ifndef FINDBLOCK_PROCESSOR_HPP_
#define FINDBLOCK_PROCESSOR_HPP_

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
namespace FindBlock {

struct FindBlock_Props: public Base::Props
{

	int d;
	int timeout;
	int len_min, len_max;
	int len_min_b, len_max_b;
	string type;

	void load(const ptree & pt)
	{
		d = pt.get("d",0.031);
		timeout = pt.get("timeout",150);
		len_min = pt.get("len_min",40);
		len_max = pt.get("len_max",200);
		len_min_b = pt.get("len_min_b",100);
		len_max_b = pt.get("len_max_b",400);
		type = pt.get("type","get_first");
	}

	void save(ptree & pt)
	{
		pt.put("d", d);
		pt.put("timeout", timeout);
		pt.put("len_min", len_min);
		pt.put("len_max", len_max);
		pt.put("len_min_b", len_min_b);
		pt.put("len_max_b", len_max_b);
		pt.put("type", type);
	}

};

class FindBlock_Processor: public Base::Component
{
public:
        FindBlock_Processor(const std::string & name = "");
        virtual ~FindBlock_Processor();

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

        void onNewColor();
        void onLineSegmentsEstimated();

		/** New image event handler. */
		Base::EventHandler <FindBlock_Processor> h_onLineSegmentsEstimated;
		Base::EventHandler <FindBlock_Processor> h_onNewColor;

		Base::DataStreamIn <uint32_t> in_color;

		/** Image stream. */
		Base::DataStreamIn <Types::Segmentation::SegmentedImage> in_lineSegmentsEstimated;

		/** Position stream. */
		Base::DataStreamOut <Types::ImagePosition> out_imagePosition;
        Base::DataStreamOut <Types::DrawableContainer> out_points;
        Base::DataStreamOut <Types::DrawableContainer> out_lines;

        /** Raised when block has been located on the image. */
        Base::Event* blockLocated;

        /** Raised when block has not been found on the image. */
        Base::Event* blockNotFound;

        double prev_gamma;
        int counter;
        int block_color;

    	FindBlock_Props props;
};
}
}

/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("FindBlock", Processors::FindBlock::FindBlock_Processor, Common::Panel_Empty)

#endif /* FINDBLOCK_PROCESSOR_HPP_ */
