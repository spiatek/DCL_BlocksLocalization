/*
 * BReading.hpp
 *
 *  Created on: 2011-11-20
 *      Author: spiatek
 */

#ifndef BREADING_HPP_
#define BREADING_HPP_

#include "Reading.hpp"

namespace Types {
namespace Mrrocpp_Proxy {

class BReading: public Reading
{
public:
	BReading()
	{
	}

	virtual ~BReading()
	{
	}

	virtual BReading* clone()
	{
		return new BReading(*this);
	}

	bool rpcReceived;

	virtual void send(boost::shared_ptr<xdr_oarchive<> > & ar){
		*ar<<*this;
	}

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object <Reading>(*this);
		LOG(LTRACE) << "BReading::serialize()\n";

		ar & rpcReceived;
	}
};

}//namespace Mrrocpp_Proxy
}//namespace Types

#endif /* BREADING_HPP_ */
