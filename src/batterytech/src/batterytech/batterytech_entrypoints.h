
#ifndef BATTERYTECH_ENTRYPOINTS_H_
#define BATTERYTECH_ENTRYPOINTS_H_

class BTApplicationRenderer {
public:
	virtual void render()=0;
};

class BTApplicationUpdater {
public:
	virtual void update()=0;
};

#endif
