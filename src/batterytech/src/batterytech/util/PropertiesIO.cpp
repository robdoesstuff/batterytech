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
// Name        : PropertiesIO.cpp
// Description : Reads and writes properties to and from text file.
//============================================================================

#include "PropertiesIO.h"
#include "../platform/platformgeneral.h"
#include "../Logger.h"
#include <string.h>
#include <stdio.h>
#include "TextFileUtil.h"

namespace BatteryTech {

	PropertiesIO::PropertiesIO() {
	}

	PropertiesIO::~PropertiesIO() {
	}

	void PropertiesIO::saveProperties(ManagedArray<Property> *properties, const char* path) {
		char myPath[255];
		_platform_convert_path(path, myPath);
		logmsg("Saving to file:");
		logmsg(myPath);
		if (!_platform_path_exists(myPath)) {
			char basepath[255];
			_platform_get_basename(myPath, basepath);
			_platform_path_create_recursive(basepath);
		}
		FILE *file = fopen(myPath, "w");
		char buf[255];
		for (S32 i = 0; i < properties->getSize(); i++) {
			Property *prop = properties->array[i];
			buf[0] = '\0';
			strcat(buf, prop->getName());
			strcat(buf, "=");
			strcat(buf, prop->getValue());
			strcat(buf, "\n");
			fwrite(buf, sizeof(char), strlen(buf), file);
		}
		fclose(file);
		logmsg("Properties Saved");

	}

	// Loads properties from a file at path.  You are responsible for freeing returned data structure.
	StrHashTable<Property*>* PropertiesIO::loadPropertiesFromFile(const char* path) {
		FILE *file = fopen(path, "rb");
		fseek(file, 0L, SEEK_END);
		S32 size = ftell(file);
		char *text = new char[size+1];
		rewind(file);
		fread(text, sizeof(char), size, file);
		fclose(file);
		text[size] = '\0';
		StrHashTable<Property*> *properties = loadPropertiesFromMemory(text);
		delete [] text;
		return properties;
	}

	// Loads properties from a file at path.  You are responsible for freeing returned data structure.
	StrHashTable<Property*>* PropertiesIO::loadPropertiesFromAsset(const char* assetName) {
		char *text = _platform_load_text_asset(assetName);
		if (text) {
			StrHashTable<Property*> *properties = loadPropertiesFromMemory(text);
			_platform_free_asset((unsigned char*)text);
			return properties;
		}
		return NULL;
	}

	StrHashTable<Property*>* PropertiesIO::loadPropertiesFromMemory(const char* text) {
		// read number of lines in file first to count
		char line[255];
		char nameBuf[255];
		char valueBuf[255];
		S32 pos = 0;
		BOOL32 isDone = FALSE;
		S32 lineCount = 0;
		while (!isDone) {
			isDone = !TextFileUtil::readLine(line, text, &pos);
			if (strlen(line) > 0) {
				lineCount++;
			}
		}
		pos = 0;
		isDone = FALSE;
		StrHashTable<Property*> *properties = new StrHashTable<Property*>(lineCount * 1.3f);
		while (!isDone) {
			isDone = !TextFileUtil::readLine(line, text, &pos);
			S32 lineLength = strlen(line);
			BOOL32 isComment = TRUE;
			for (S32 i = 0; i < lineLength; i++) {
				if (line[i] != ' ' && line[i] != '\t') {
					if (line[i] != '#') {
						isComment = FALSE;
					}
					break;
				}
			}
			if (strlen(line) > 0 && !isComment) {
				logmsg(line);
				// find equals sign, break into before and after.
				char *sepPtr = strchr(line, '=');
				strncpy(nameBuf, line, sepPtr-line);
				nameBuf[sepPtr-line] = '\0';
				strncpy(valueBuf, sepPtr+1, strlen(line)-(sepPtr-line));
				valueBuf[strlen(line)-(sepPtr-line)] = '\0';
				properties->put(nameBuf, new Property(nameBuf, valueBuf));
			}
		}
		return properties;
	}
}
