/*
 * InputStream.h
 *
 *  Created on: 17 May 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_INPUTSTREAM_H_
#define APP_UTILS_INPUTSTREAM_H_

namespace od
{

	class InputStream
	{
	public:
		virtual ~InputStream();

		virtual int read(char *buffer, int len) = 0;
		virtual int seek(int offset) = 0;
		virtual int tell() = 0;
		virtual int getSize() = 0;
	};

} /* namespace od */

#endif /* APP_UTILS_INPUTSTREAM_H_ */
