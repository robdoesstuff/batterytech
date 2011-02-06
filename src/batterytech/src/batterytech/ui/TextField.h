/*
 * Label.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef TEXTFIELD_H_
#define TEXTFIELD_H_

#include "UIComponent.h"

class TextField: public UIComponent {
public:
	TextField(const char *text = NULL) : UIComponent(text){ allowNumeric = TRUE; };
	virtual ~TextField();
	virtual void onKeyPressed(U8 key);
	virtual void onSelected();
	void setAllowNumeric(BOOL32 allowNumeric) {this->allowNumeric = allowNumeric;};
private:
	void addChar(U8 key);
	void deleteChar();
	BOOL32 allowNumeric;
};

#endif /* LABEL_H_ */
