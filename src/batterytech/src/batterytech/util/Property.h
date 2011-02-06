/*
 * Property.h
 *
 *  Created on: Jan 3, 2011
 *      Author: rgreen
 */

#ifndef PROPERTY_H_
#define PROPERTY_H_

class Property {
public:
	Property(const char* name, const char* value);
	virtual ~Property();
	const char* getName();
	const char* getValue();
	void setName(const char* name);
	void setValue(const char* value);
private:
	const char* name;
	const char* value;
};

#endif /* PROPERTY_H_ */
