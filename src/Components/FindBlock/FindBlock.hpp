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
#include "Property.hpp"
#include "EventHandler2.hpp"

#include "Types/SegmentedImage.hpp"
#include "Types/DrawableContainer.hpp"
#include "Types/Drawable.hpp"
#include "Types/ImagePosition.hpp"

#define	 AVERAGE	1
#define NEAREST		2

namespace Processors {
namespace FindBlock {

class FindBlockTranslator {
public:
	static int fromStr(const std::string & s)
	{
		if(s == "AVERAGE")		return AVERAGE;
		if(s == "NEAREST")		return NEAREST;
								return NEAREST;
	}

	static std::string toStr(int t)
	{
		switch(t)
		{
			case AVERAGE:	return "AVERAGE";
			case NEAREST:		return "NEAREST";
			default:			return "NEAREST";
		}
	}
};

class FindBlock_Processor: public Base::Component
{
public:
        FindBlock_Processor(const std::string & name = "");
        virtual ~FindBlock_Processor();

        void prepareInterface();

//    	Base::Props * getProperties()
//    	{
//    		return &props;
//    	}

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
		//Base::EventHandler <FindBlock_Processor> h_onLineSegmentsEstimated;
		//Base::EventHandler <FindBlock_Processor> h_onNewColor;
		Base::EventHandler2 h_onLineSegmentsEstimated;
		Base::EventHandler2 h_onNewColor;

		Base::DataStreamIn <uint32_t> in_color;

		/** Image stream. */
		Base::DataStreamIn <Types::Segmentation::SegmentedImage> in_lineSegmentsEstimated;

		/** Position stream. */
		Base::DataStreamOut <Types::ImagePosition> out_imagePosition;
        Base::DataStreamOut <Types::DrawableContainer> out_points;
        Base::DataStreamOut <Types::DrawableContainer> out_lines;

        /** Raised when block has been located on the image. */
        //Base::Event* blockLocated;

        /** Raised when block has not been found on the image. */
        //Base::Event* blockNotFound;

        double prev_gamma;
        int counter;
        int block_color;

    	Base::Property<int> max_iterations;
    	Base::Property<int> l_min_block;
    	Base::Property<int> l_max_block;
    	Base::Property<int> l_min_board;
    	Base::Property<int> l_max_board;
    	Base::Property<int, FindBlockTranslator> radian_opt;
};
}
}

/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("FindBlock", Processors::FindBlock::FindBlock_Processor, Common::Panel_Empty)

#endif /* FINDBLOCK_PROCESSOR_HPP_ */
