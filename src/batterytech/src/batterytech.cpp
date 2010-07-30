//============================================================================
// Name        : batterytech.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "batterytech.h"

F32 theta = 0.0f;

/*
int main() {
	cout << "!!!Hello World!!!" << endl;
	if (DEBUG) {
		cout << "Debugging is enabled" << endl;
	}
	return 0;
}
*/

void btInit() {
	log("BatteryTech 1.0 Initializing...");
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	log("Ready");
}

void btUpdate(F32 delta) {
	theta += delta * 60;
	//char buf[32];
	//sprintf(buf, "update %3.5g theta is %3.5g", delta, theta);
	//log(buf);
}


void btDraw() {
	// OpenGL animation code goes here
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glViewport(0,0,800,480);
	glLoadIdentity();
	glOrthof(-1.6f, 1.6f, -.96f, .96f, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glColor4f(1, 1, 1, 1);
	glPushMatrix();
	glRotatef( theta, 0.0f, 0.0f, 1.0f );
	glFrontFace(GL_CW);
	float colors[] = {
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f
	};
	float verts[] = {
			0.0f, 1.0f, 0.0f,
			0.87f, -0.5f, 0.0f,
			-0.87f, -0.5f, 0.0f,
	};
	glColorPointer(4, GL_FLOAT, 0, &colors);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glPopMatrix();
}
