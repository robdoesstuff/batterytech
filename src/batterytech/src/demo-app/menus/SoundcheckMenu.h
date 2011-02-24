/*
 * HighScoresMenu.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#ifndef SOUNDCHECKMENU_H_
#define SOUNDCHECKMENU_H_

#include <batterytech/Context.h>
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Label.h>
#include <batterytech/ui/Button.h>
#include <batterytech/ui/Checkbox.h>
#include <batterytech/util/ManagedArray.h>

#define SOUNDCHECK_MENU_NAME "SoundcheckMenu"

class SoundcheckMenu : public Menu {
public:
	SoundcheckMenu(Context *context);
	virtual ~SoundcheckMenu();
	virtual void onClickUp(UIComponent *component);
	virtual void onPreShow();
	virtual void setData(void *data);
private:
	Context *context;
	Label *titleLabel;
	Button *backButton;
	Button *streamButton;
	Button *sound1Button;
	Button *sound2Button;
	Button *sound3Button;
	Button *sound4Button;
	LinearLayout *mainLayout;
	BOOL32 isStreaming;
};

#endif /* SOUNDCHECKMENU_H_ */
