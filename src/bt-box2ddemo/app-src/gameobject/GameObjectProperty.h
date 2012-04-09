/*
 * GameObjectProperty.h
 *
 *  Created on: Dec 12, 2010
 *      Author: rgreen
 */

#ifndef GAMEOBJECTPROPERTY_H_
#define GAMEOBJECTPROPERTY_H_

class GameObjectProperty {
public:
	GameObjectProperty(const char* name, const char* value);
	virtual ~GameObjectProperty();
	void setValue(const char* value);
	char* name;
	char* value;
};

#endif /* GAMEOBJECTPROPERTY_H_ */
