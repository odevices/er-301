/*
 * Port.h
 *
 *  Created on: 10 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_PORT_H_
#define APP_OBJECTS_PORT_H_

#include <od/extras/ReferenceCounted.h>
#include <stdint.h>
#include <string>

namespace od
{

	class Port : public ReferenceCounted
	{
	public:
#ifndef SWIGLUA
		Port();
		Port(const std::string &name);
		Port(const std::string &name, uint32_t index);
		virtual ~Port();

		std::string mName;
		uint32_t mIndex = 0;
#endif

		const std::string &name();
	};

} /* namespace od */

#endif /* APP_OBJECTS_PORT_H_ */
