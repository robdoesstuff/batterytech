/*
 * GLObjSceneBinding.h
 *
 *  Created on: Jul 12, 2011
 *      Author: rgreen
 */

#ifndef GLOBJSCENEBINDING_H_
#define GLOBJSCENEBINDING_H_

#include "../util/ManagedArray.h"
#include "../primitives.h"

namespace BatteryTech {

class GLModelBinding;
class ObjScene;
class Context;

class GLObjSceneBinding {
public:
	GLObjSceneBinding(const char *objAssetName);
	virtual ~GLObjSceneBinding();
	void load(Context *context);
	void unload(Context *context);
	char *objAssetName;
	ObjScene *objScene;
	ManagedArray<GLModelBinding> *modelBindingArray;
};

}
#endif /* GLOBJSCENEBINDING_H_ */
