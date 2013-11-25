/*
 * FindBlock.cpp
 *
 *  Created on: 22-10-2011
 *      Author: spiatek
 */

#include <memory>
#include <string>
#include <cmath>

#include <boost/bind.hpp>

#include "FindBlock.hpp"
#include "Common/Logger.hpp"

#define BOARD_COLOR 5
//#define BLOCK_WIDTH 0.031

#define A 1655
#define B 1.4675
#define C -0.9837

//size_t line_length(Types::Line*);

namespace Processors {
namespace FindBlock {

using namespace Types;
using Types::Segmentation::SegmentedImage;

FindBlock_Processor::FindBlock_Processor(const std::string & name) :
        Base::Component(name),
        type("type", NEAREST, "combo"),
        max_iterations("max_iterations", 150, "range"),
        l_min_block("l_min_block", 40, "range"),
        l_max_block("l_max_block", 200, "range"),
        l_min_board("l_min_board", 100, "range"),
        l_max_board("l_max_board", 400, "range")
{
        LOG(LTRACE) << "Hello FindBlock_Processor\n";
}

FindBlock_Processor::~FindBlock_Processor()
{
        LOG(LTRACE) << "Good bye FindBlock_Processor\n";
}

void FindBlock_Processor::prepareInterface()
{
    LOG(LTRACE) << "FindBlock_Processor::prepareInterface\n";

    registerStream("in_color", &in_color);
    registerStream("in_lineSegmentsEstimated", &in_lineSegmentsEstimated);

    registerStream("out_imagePosition", &out_imagePosition);
    //registerStream("out_points", &out_points);
    //registerStream("out_lines", &out_lines);

    h_onLineSegmentsEstimated.setup(boost::bind(&FindBlock_Processor::onLineSegmentsEstimated, this));
    registerHandler("onLineSegmentsEstimated", &h_onLineSegmentsEstimated);
    addDependency("onLineSegmentsEstimated", &in_lineSegmentsEstimated);

    h_onNewColor.setup(boost::bind(&FindBlock_Processor::onNewColor, this));
    registerHandler("onNewColor", &h_onNewColor);
    addDependency("onNewColor", &in_color);

    //blockLocated = registerEvent("blockLocated");
    //blockNotFound = registerEvent("blockNotFound");

    prev_gamma = 0;
    counter = max_iterations + 1;
    block_color = BOARD_COLOR;
}

bool FindBlock_Processor::onInit()
{
        LOG(LTRACE) << "FindBlock_Processor::initialize\n";

        return true;
}

bool FindBlock_Processor::onFinish()
{
        LOG(LTRACE) << "FindBlock_Processor::finish\n";

        return true;
}

bool FindBlock_Processor::onStep()
{
        LOG(LTRACE) << "FindBlock_Processor::step\n";
        return true;
}

bool FindBlock_Processor::onStop()
{
        return true;
}

bool FindBlock_Processor::onStart()
{
        return true;
}


void FindBlock_Processor::onNewColor()
{
	LOG(LTRACE) << "FindBlock_Processor::onNewColor()\n";

	block_color = in_color.read();
}

void FindBlock_Processor::onLineSegmentsEstimated()
{
	//LOG(LTRACE) << "FindBlock_Processor::onLineSegmentsEstimated()\n";
	//LOG(LNOTICE) << "FindBlock_Processor::onLineSegmentsEstimated()\n";

	try {

		//Input and output types and containers
		Types::Segmentation::SegmentedImage si = in_lineSegmentsEstimated.read();

		Types::ImagePosition imagePosition;
		Types::DrawableContainer dc;
		Types::DrawableContainer ol;

		//Compute image center
		cv::Mat img = si.image;
		size_t si_rows = img.size().height;
		size_t si_cols = img.size().width;
		cv::Point* pos_centr = new cv::Point(si_cols/2, si_rows/2);

		//Get filtration parameters from xml file
		size_t l_min, l_max;

		if(block_color == BOARD_COLOR) {
			l_min = l_min_board;
			l_max = l_max_board;
		}
		else {
			l_min = l_min_block;
			l_max = l_max_block;
		}

		if(counter > 0) {
			counter -= 1;
		}
		else {
			counter = max_iterations;
		}

		//Local vectors
		std::vector<Types::Segmentation::Segment> active_segments;	//vector for active segments (after filtration)
		std::vector<cv::Point> segment_centers;						//vector for segment centers
		std::vector<double> ls_rotations;							//vector for line segment's rotations

		//For each segment in segmented image
		for(size_t i = 0; i < si.segments.size(); ++i) {
			std::vector<Types::Line>* lines = si.segments[i].getLineSegments();
			size_t sum_x = 0, sum_y = 0, l_length = 0;

			std::vector<int> indexes;	//save indexes of chosen lines
			int wo = 0;					//wo = 1 if segment is too large

			//For each line in segment
			for(size_t j = 0; j < lines->size(); ++j) {

				//Compute length of a line
				cv::Point p1 = ((*lines)[j]).getP1();
				cv::Point p2 = ((*lines)[j]).getP2();
				l_length = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

				//Eliminate too large segments
				if(l_length > l_max) {
					wo = 1;
					break;
				}

				//Lines reduction
				if(l_length < l_max && l_length > l_min) {
					indexes.push_back(j);
					sum_x += (p1.x + p2.x);
					sum_y += (p1.y + p2.y);
				}

			}

			LOG(LNOTICE) << indexes.size() << ", " << wo;

			//Get only segments consisted of four lines
			if(indexes.size() == 4 && wo == 0)
			{

				//Compute segment's center coordinates
				cv::Point* pos_abs = new cv::Point(sum_x/(2*indexes.size()),sum_y/(2*indexes.size()));
				Types::Line* line_abs = new Types::Line(*pos_abs, *pos_centr);



				for(size_t j = 0; j < lines->size(); ++j)
				{
					//Compute line length
					cv::Point p1 = ((*lines)[j]).getP1();
					cv::Point p2 = ((*lines)[j]).getP2();
					l_length = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

					if(l_length > l_min && l_length < l_max) {

						//Draw lines connecting segment's center with image center
						dc.add(line_abs);

						//Compute a radian between block's contour and optic axis of a camera
						long double y;
						if((p2.x > p1.x && p2.y < p1.y) || (p1.x > p2.x && p1.y < p2.y)) {
							y = atan2((long double)(p1.y - p2.y), (long double)(p1.x - p2.x));
						}
						else {
							y = atan2((long double)(p1.y - p2.y), (long double)(p1.x - p2.x)) - M_PI/2;		//1nd correction
						}
						ls_rotations.push_back(y);
						Types::Line* line = new Line((*lines)[j]);
						ol.add(line);
					}

				}

				//Add segment to image's vector of blocks
				active_segments.push_back(si.segments[i]);

				//Add segment's center to vector of block centers
				segment_centers.push_back(*pos_abs);
			}
		}

		//Compute servo parameters
		if(active_segments.size() > 0) {

			double maxPixels = std::max(si.image.size().width, si.image.size().height);

			//Compute average of segment rotations
			double im_g = 0.0;				//imagePosition structure parameter (segment rotation)
			double lowest = M_PI;			//minimal difference between im_g computed in this and previous iteration
			double nearest_prev = M_PI;		//value closest to im_g computed in previous iteration
			double nearest_0 = M_PI;		//value closest to 0

			for(size_t z = 0; z < ls_rotations.size(); ++z) {

				double act_r;				//potential corrected radian value (between -PI and PI)
				double it_r; 				//difference between act_r and previous value of im_g
				double act_min = M_PI;		//minimum of it_r
				double act_gamma;			//radian value closest to previous value if im_g

				for(int i = -2; i < 2; ++i) {			//get one of radian values between -PI and PI
					act_r = ls_rotations[z] + i*M_PI/2;
					it_r = abs(act_r - prev_gamma);
					if(it_r < act_min) {
						act_min = it_r;
						act_gamma = act_r;
					}
					if(abs(act_r) < nearest_0) {
						nearest_0 = act_r;
					}
				}

				ls_rotations[z] = act_gamma;		//fill ls_rotations[z] with value closest to prev_gamma (2nd correction)

				if(act_min < lowest) {
					lowest = act_min;
					nearest_prev = act_gamma;
				}
			}

			if(ls_rotations.size() >= 4) {

				//first iteration detection
				if(counter == max_iterations + 1) {
					im_g = nearest_0;
				}

				//take avarage twice corrected radian value from first segment
				else if(type == AVERAGE) {
					im_g = (ls_rotations[0] + ls_rotations[1] + ls_rotations[2] + ls_rotations[3])/4;
				}

				//take value closest to prev_gamma
				else {
					im_g = nearest_prev;
				}

				prev_gamma = im_g;
			}
			//cout << im_g << endl;

			//Compute distance between block and camera
			//double im_z = C + A/(B + d);

			//Compute servo X, Y parameters
			int im_x = 0, im_y = 0;
/*			if(type == "average") {		//compute average
				int sumx = 0, sumy = 0;
				for(size_t z = 0; z < segment_centers.size(); ++z) {
					sumx += segment_centers[z].x - pos_centr->x;
					sumy += segment_centers[z].y - pos_centr->y;
				}
				if(segment_centers.size() > 0) {
					im_x = sumx/segment_centers.size();
					im_y = sumy/segment_centers.size();
				}
			}
			else */
			if(segment_centers.size() > 0)	//get first segment
			{
				im_x = segment_centers[0].x - pos_centr->x;
				im_y = segment_centers[0].y - pos_centr->y;
			}
			//cout << im_x/maxPixels << ", " << im_y/maxPixels << endl;

			//Set servo parameters
			imagePosition.elements[0] = im_x/maxPixels;
			imagePosition.elements[1] = im_y/maxPixels;
			imagePosition.elements[2] = 0;
			imagePosition.elements[3] = im_g;

			LOG(LNOTICE) << "Image position: " << im_x/maxPixels << " " << im_y/maxPixels << " | " << im_g << "\n";

			//Prepare output data
			out_imagePosition.write(imagePosition);
			//out_points.write(dc);
			//out_lines.write(ol);

			//blockLocated->raise();
		}
		else {
			//blockNotFound->raise();
		}
	}
	catch (const Common::DisCODeException& e) {
		LOG(LERROR) << e.what() << "\n";
	}
	LOG(LTRACE) << "void FindBlock_Processor::onLineSegmentsEstimated() end\n";}

}

}
