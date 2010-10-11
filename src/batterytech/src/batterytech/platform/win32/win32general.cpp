/*
 * win32general.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_CPP_
#define WIN32GENERAL_CPP_
#ifdef _WIN32

#include "win32general.h"
#include "SoundServer.h"
#include "../../sound/SoundManager.h"
#include <stdio.h>

using namespace std;

static SoundManager *sndMgr;

void _convert_filename(char *filename);

void _platform_log(const char* message) {
	cout << message << endl;
}

unsigned char* _platform_load_asset(const char *filename, int *size) {
	//char *myFilename;
	//strcpy(myFilename, filename);
	//_convert_filename(myFilename);
	char myFilename[7 + strlen(filename)];
	strcpy(myFilename, "assets\\");
	strcat(myFilename, filename);
	cout << "trying " << myFilename << endl;
	FILE *handle;
	unsigned char *data = 0;
	handle = fopen(myFilename, "rb");
	if (!handle) {
		cout << "No file handle" << endl;
	}
	fseek(handle, 0L, SEEK_END);
	*size = ftell(handle);
	//fseek(handle, 0L, SEEK_SET);
	rewind(handle);
	data = (unsigned char*) malloc(sizeof(unsigned char) * *size);
	if (data) {
		int bytesRead = fread(data, sizeof(unsigned char), *size, handle);
		cout << "malloc success, " << bytesRead << " bytes read of " << *size << endl;
	}
	int error = ferror(handle);
	if (error) {
		cout << "IO error " << error << endl;
	}
	if (feof(handle)) {
		cout << "EOF reached " << endl;
	}
	fclose(handle);
	return data;
}

void _platform_free_asset(unsigned char *ptr) {
	if (ptr) {
		free(ptr);
	}
}

void _convert_filename(char *filename) {
	int arrayLength = sizeof(filename);
	int i;
	for (i = 0; i < arrayLength; i++) {
		if (filename[i] == '/') {
			filename[i] = '\\';
		}
	}
}

#define	WND_CLASSNAME	"SoundWindow"
#define	TWO_PI			(3.1415926f * 2.f)
#define	SIN_STEP		((TWO_PI * 440.f) / 44100.f)
static	float			sinPos = 0.f;

void mySoundProc(void *pSoundBuffer,long bufferLen)
{
	if (sndMgr) {
		sndMgr->fillBuffer(pSoundBuffer, bufferLen);
	}
	//cout << "Buffering " << bufferLen << " bytes" << endl;
	// Convert params, assuming we create a 16bits, mono waveform.
}

void _platform_init_sound(SoundManager *soundManager) {
	sndMgr = soundManager;
	CSoundServer *pServer = new CSoundServer;
	if (pServer->open(mySoundProc)) {
		cout << "Windows Sound Started" << endl;
	}
}

#endif /* _WIN32 */
#endif /* WIN32GENERAL_CPP_ */
