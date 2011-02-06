/*
 * Property.cpp
 *
 *  Created on: Jan 3, 2011
 *      Author: rgreen
 */

#include "Property.h"
#include "strx.h"

Property::Property(const char* name, const char* value) {
	this->name = strDuplicate(name);
	this->value = strDuplicate(value);
}

Property::~Property() {
	delete name;
	name = NULL;
	delete value;
	value = NULL;
}

const char* Property::getName() {
	return name;
}

const char* Property::getValue() {
	return value;
}

void Property::setName(const char* name) {
	if (name) {
		delete name;
	}
	this->name = strDuplicate(name);
}

void Property::setValue(const char* value) {
	if (value) {
		delete value;
	}
	this->value = strDuplicate(value);
}
