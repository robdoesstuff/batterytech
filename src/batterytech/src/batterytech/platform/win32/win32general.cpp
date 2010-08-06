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

using namespace std;

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

#endif /* _WIN32 */
#endif /* WIN32GENERAL_CPP_ */
