/*
 * FindBlock.cpp
 *
 *  Created on: 22-10-2011
 *      Author: spiatek
 */

#include <memory>
#include <string>

#include "FindBlock.hpp"
#include "Common/Logger.hpp"

size_t line_length(Types::Line*);

namespace Processors {
namespace FindBlock {

using namespace Types;
using Types::Segmentation::SegmentedImage;

FindBlock_Processor::FindBlock_Processor(const std::string & name) :
        Base::Component(name)
{
        LOG(LTRACE) << "Hello FindBlock_Processor\n";
}

FindBlock_Processor::~FindBlock_Processor()
{
        LOG(LTRACE) << "Good bye FindBlock_Processor\n";
}

bool FindBlock_Processor::onInit()
{
        LOG(LTRACE) << "FindBlock_Processor::initialize\n";

        h_onLineSegmentsEstimated.setup(this, &FindBlock_Processor::onLineSegmentsEstimated);
        registerHandler("onLineSegmentsEstimated", &h_onLineSegmentsEstimated);

        registerStream("in_lineSegmentsEstimated", &in_lineSegmentsEstimated);
        registerStream("out_imagePosition", &out_imagePosition);
        registerStream("out_points", &out_points);
        registerStream("out_lines", &out_lines);

        blockLocated = registerEvent("blockLocated");
        blockNotFound = registerEvent("blockNotFound");
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

void FindBlock_Processor::onLineSegmentsEstimated()
{
	LOG(LTRACE) << "FindBlock_Processor::onLineSegmentsEstimated()\n";
	LOG(LNOTICE) << "FindBlock_Processor::onLineSegmentsEstimated()\n";

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
		size_t l_min = props.len_min;
		size_t l_max = props.len_max;
		string type = props.type;

		//Local vectors
		std::vector<Types::Segmentation::Segment> active_blocks;	//vector for active segments (after filtration)
		std::vector<cv::Point> pabs_v;								//vector for segment centers
		std::vector<double> ys;										//vector for segment rotations

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
							ys.push_back(y);
							Types::Line* line = new Line((*lines)[j]);
							ol.add(line);
						}

					}

				}

				//Add segment to image's vector of blocks
				active_blocks.push_back(si.segments[i]);

				//Add segment's center to vector of block centers
				pabs_v.push_back(*pos_abs);
			}
		}

		//Compute servo parameters
		if(active_blocks.size() > 0) {

			double maxPixels = std::max(si.image.size().width, si.image.size().height);

			//Compute average of segment rotations
			double sumg = 0.0, im_g = 0.0;
			for(size_t z = 0; z < ys.size(); ++z) {
				sumg = sumg + (double) ys[z];
			}
			if(ys.size() > 0) {
				im_g = sumg/ys.size();
			}
			//cout << im_g << endl;

			//Compute servo X, Y parameters
			int im_x = 0, im_y = 0;
			if(type == "average") {		//compute average
				int sumx = 0, sumy = 0;
				for(size_t z = 0; z < pabs_v.size(); ++z) {
					sumx += pabs_v[z].x - pos_centr->x;
					sumy += pabs_v[z].y - pos_centr->y;
				}
				if(pabs_v.size() > 0) {
					im_x = sumx/pabs_v.size();
					im_y = sumy/pabs_v.size();
				}
			}
			else if(pabs_v.size() > 0)	//get first segment
			{
				im_x = pabs_v[0].x - pos_centr->x;
				im_y = pabs_v[0].y - pos_centr->y;
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
			out_points.write(dc);
			out_lines.write(ol);

			blockLocated->raise();
		}
		else {
			blockNotFound->raise();
		}
	}
	catch (const Common::DisCODeException& e) {
		LOG(LERROR) << e.what() << "\n";
	}
	LOG(LTRACE) << "void FindBlock_Processor::onLineSegmentsEstimated() end\n";}

}
}
