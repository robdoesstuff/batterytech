/*
 * GameObjectProperty.cpp
 *
 *  Created on: Dec 12, 2010
 *      Author: rgreen
 */

#include "GameObjectProperty.h"
#include <string.h>

GameObjectProperty::GameObjectProperty(const char* name, const char* value) {
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->value = new char[strlen(value) + 1];
	strcpy(this->value, value);
}

GameObjectProperty::~GameObjectProperty() {
	delete name;
	delete value;
}

void GameObjectProperty::setValue(const char* value) {
	if (this->value) {
		delete value;
	}
	this->value = new char[strlen(value) + 1];
	strcpy(this->value, value);
}
