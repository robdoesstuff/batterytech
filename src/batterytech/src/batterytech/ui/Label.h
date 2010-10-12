/*
 * Label.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef LABEL_H_
#define LABEL_H_

#include "UIComponent.h"

class Label: public UIComponent {
public:
	Label(const char *text = NULL) : UIComponent(text){};
	virtual ~Label();
};

#endif /* LABEL_H_ */
