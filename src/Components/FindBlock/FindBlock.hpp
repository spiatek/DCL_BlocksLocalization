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

	int max_iterations;
	int l_min_block, l_max_block;
	int l_min_board, l_max_board;
	string radian_opt;

	void load(const ptree & pt)
	{
		max_iterations = pt.get("max_iterations",150);
		l_min_block = pt.get("l_min_block",40);
		l_max_block = pt.get("l_max_block",200);
		l_min_board = pt.get("l_min_board",100);
		l_max_board = pt.get("l_max_board",400);
		radian_opt = pt.get("radian_opt","nearest");
	}

	void save(ptree & pt)
	{
		pt.put("max_iterations", max_iterations);
		pt.put("l_min_block", l_min_block);
		pt.put("l_max_block", l_max_block);
		pt.put("l_min_board", l_min_board);
		pt.put("l_max_board", l_max_board);
		pt.put("radian_opt", radian_opt);
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
