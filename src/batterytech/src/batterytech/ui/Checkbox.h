/*
 * Checkbox.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include "LinearLayout.h"
#include "../primitives.h"
#include "Button.h"
#include "Label.h"

class Checkbox: public LinearLayout {
public:
	Checkbox(const char *text = NULL);
	void setBoxResourceId(S32 resourceId);
	void setCheckedResourceId(S32 resourceId);
	virtual void onClickDown();
	virtual void onClickUp();
	virtual void setText(const char *text);
	virtual void update(F32 delta);
	virtual ~Checkbox();
private:
	Button *button;
	Label *label;
	S32 boxResourceId;
	S32 checkedResourceId;
	BOOL32 checked;
};

#endif /* CHECKBOX_H_ */
