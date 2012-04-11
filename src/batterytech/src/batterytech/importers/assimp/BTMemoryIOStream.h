/*
 * BTMemoryIOStream.h
 *
 *  Created on: Oct 12, 2011
 *      Author: rgreen
 */

#ifndef BTMEMORYIOSTREAM_H_
#define BTMEMORYIOSTREAM_H_

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "../../../bt-assimp/include/IOStream.h"
#include "../../primitives.h"

namespace BatteryTech {

class BTMemoryIOStream: public Assimp::IOStream {
public:
	BTMemoryIOStream(unsigned char *data, S32 size);
	virtual ~BTMemoryIOStream();
	virtual size_t Read(void* pvBuffer, size_t pSize, size_t pCount);
	virtual size_t Write(const void* pvBuffer, size_t pSize, size_t pCount);
	virtual aiReturn Seek(size_t pOffset, aiOrigin pOrigin);
	virtual size_t Tell() const;
	virtual size_t FileSize() const;
	virtual void Flush();
private:
	unsigned char *data;
	S32 dataSize;
	S32 pos;
};

}

#endif /* BTMEMORYIOSTREAM_H_ */

#endif
