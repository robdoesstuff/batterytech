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
// Name        : PropertiesIO.h
// Description : Reads and writes properties to and from text file.
//============================================================================

#ifndef PROPERTIESIO_H_
#define PROPERTIESIO_H_

#include "ManagedArray.h"
#include "HashTable.h"
#include "Property.h"

namespace BatteryTech {

	class PropertiesIO {
	public:
		PropertiesIO();
		virtual ~PropertiesIO();
		/**
		 * Saves an array of properties to disk
		 */
		void saveProperties(ManagedArray<Property> *properties, const char* path);
		/**
		 * Loads from disk an array of properties
		 */
		StrHashTable<Property*>* loadPropertiesFromFile(const char* path);
		StrHashTable<Property*>* loadPropertiesFromAsset(const char* assetName);
	private:
		StrHashTable<Property*>* loadPropertiesFromMemory(const char* text);
	};

}

#endif /* PROPERTIESIO_H_ */
