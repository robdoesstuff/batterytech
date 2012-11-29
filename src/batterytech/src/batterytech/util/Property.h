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
// Name        : Property.h
// Description : A name/value text property.  Use with PropertiesIO class.
//============================================================================

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include "../math/Vector4.h"

namespace BatteryTech {

	class Property {
	public:
		Property(const char* name, const char* value);
		virtual ~Property();
		/**
		 * Returns the name of the property
		 */
		const char* getName();
		/**
		 * Returns the value of the property
		 */
		const char* getValue();
		/**
		 * Returns the value of the property as an integer
		 */
		int getIntValue();
		/**
		 * Returns the value of the property as a numeric boolean
		 */
		int getBoolValue();
		/**
		 * Returns the value of the property as a float
		 */
		float getFloatValue();
		/**
		 * Sets the name of the property
		 */
		void setName(const char* name);
		/**
		 * Sets the value of the property
		 */
		void setValue(const char* value);
		/**
		 * Returns the value of the property as a Vector4f
		 */
		Vector4f getVector4fValue();
	private:
		char* name;
		char* value;
	};

}

#endif /* PROPERTY_H_ */
