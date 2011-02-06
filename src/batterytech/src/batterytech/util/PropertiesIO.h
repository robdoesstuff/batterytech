/*
 * PropertiesIO.h
 *
 *  Created on: Jan 3, 2011
 *      Author: rgreen
 */

#ifndef PROPERTIESIO_H_
#define PROPERTIESIO_H_

#include "ManagedArray.h"
#include "Property.h"

class PropertiesIO {
public:
	PropertiesIO();
	virtual ~PropertiesIO();
	void saveProperties(ManagedArray<Property> *properties, const char* path);
	ManagedArray<Property>* loadPropertiesFromFile(const char* path);
};

#endif /* PROPERTIESIO_H_ */
