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
}

void btUpdate(F32 delta) {
	// cout << "update " << delta << endl;
	theta += delta * 60;
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
	glPushMatrix();
	glRotatef( theta, 0.0f, 0.0f, 1.0f );
	float colors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
	};
	float verts[] = {
			0.0f, 1.0f,
			0.87f, -0.5f,
			-0.87f, -0.5f
	};
	glEnable(GL_COLOR_ARRAY);
	glEnable(GL_VERTEX_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glPopMatrix();
}
