/*
 * BTMemoryIOStream.cpp
 *
 *  Created on: Oct 12, 2011
 *      Author: rgreen
 */

#include "BTMemoryIOStream.h"
#include "../../primitives.h"
#include "../../platform/platformgeneral.h"
#include "../../Logger.h"

namespace BatteryTech {

BTMemoryIOStream::BTMemoryIOStream(unsigned char *data, S32 size) {
	this->data = data;
	this->dataSize = size;
	this->pos = 0;
}

BTMemoryIOStream::~BTMemoryIOStream() {
	if (data) {
		_platform_free_asset(data);
	}
	data = NULL;
	pos = 0;
	dataSize = 0;
}

size_t BTMemoryIOStream::Read(void* pvBuffer, size_t pSize, size_t pCount) {
	if (!data) {
		return 0;
	}
	S32 amt = pSize * pCount;
	if (amt + pos > dataSize) {
		amt = dataSize - pos - 1;
	}
	memcpy(pvBuffer, data + pos, amt);
	pos += amt;
	return amt / pSize;
}

size_t BTMemoryIOStream::Write(const void* pvBuffer, size_t pSize, size_t pCount) {
	return 0;
}

aiReturn BTMemoryIOStream::Seek(size_t pOffset, aiOrigin pOrigin) {
	if (pOrigin == aiOrigin_SET) {
		pos = pOffset;
	} else if (pOrigin == aiOrigin_CUR) {
		pos += pOffset;
		if (pos > dataSize) {
			pos = dataSize - 1;
		}
	} else if (pOrigin == aiOrigin_END) {
		pos = dataSize - 1 + pOffset;
		if (pos > dataSize) {
			pos = dataSize - 1;
		}
	}
	return aiReturn_SUCCESS;
}

size_t BTMemoryIOStream::Tell() const  {
	return pos;
}

size_t BTMemoryIOStream::FileSize() const {
	return dataSize;
}

void BTMemoryIOStream::Flush() {
}

}
