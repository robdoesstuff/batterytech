/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : Property.cpp
// Description : A name/value text property.  Use with PropertiesIO class.
//============================================================================

#include "Property.h"
#include "strx.h"
#include <stdlib.h>

namespace BatteryTech {

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

	int Property::getIntValue() {
		return atoi(value);
	}

	int Property::getBoolValue() {
		if (strEquals(value, "true") || strEquals(value, "TRUE") || atoi(value)) {
			return 1;
		}
		return 0;
	}

	float Property::getFloatValue() {
		return atof(value);
	}

	void Property::setName(const char* name) {
		if (this->name) {
			delete [] this->name;
		}
		this->name = strDuplicate(name);
	}

	void Property::setValue(const char* value) {
		if (this->value) {
			delete [] this->value;
		}
		this->value = strDuplicate(value);
	}

	Vector4f Property::getVector4fValue() {
		if (value) {
			char *tok = strtok(value, " ");
			Vector4f v;
			int i = 0;
			while (tok && i < 4) {
				v[i] = atof(tok);
				tok = strtok(NULL, " ");
				i++;
			}
			return v;
		}
		return Vector4f(0,0,0,0);
	}

}
