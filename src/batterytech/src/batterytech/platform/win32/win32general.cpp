/*
 * win32general.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_CPP_
#define WIN32GENERAL_CPP_
#ifdef _WIN32

#include "../platformgeneral.h"
#include "WinSound.h"
#include "../../audio/AudioManager.h"
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include "../../primitives.h"
#include <string.h>

using namespace std;
using namespace BatteryTech;

static AudioManager *sndMgr;
static WinSound *winSound;

void _convert_filename(char *filename);

void _platform_log(const char* message) {
	cout << message << endl;
}

unsigned char* _platform_load_asset(const char *filename, S32 *size) {
	//char *myFilename;
	//strcpy(myFilename, filename);
	//_convert_filename(myFilename);
	char myFilename[255];
	strcpy(myFilename, "assets\\");
	strcat(myFilename, filename);
	_platform_convert_path(myFilename, myFilename);
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

S32 _platform_get_asset_length(const char *filename) {
	char myFilename[255];
	strcpy(myFilename, "assets\\");
	strcat(myFilename, filename);
	_platform_convert_path(myFilename, myFilename);
	FILE *handle;
	handle = fopen(myFilename, "rb");
	S32 size = 0;
	if (!handle) {
		cout << "No file handle" << endl;
	} else {
		fseek(handle, 0L, SEEK_END);
		size = ftell(handle);
		fclose(handle);
	}
	return size;
}

S32 _platform_read_asset_chunk(const char *filename, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof) {
	char myFilename[255];
	strcpy(myFilename, "assets\\");
	strcat(myFilename, filename);
	_platform_convert_path(myFilename, myFilename);
	FILE *handle;
	handle = fopen(myFilename, "rb");
	if (!handle) {
		cout << "No file handle" << endl;
		return 0;
	} else {
		fseek(handle, offset, SEEK_SET);
		int bytesRead = fread(buffer, sizeof(unsigned char), bufferLength, handle);
		int error = ferror(handle);
		if (error) {
			cout << "IO error " << error << endl;
		}
		if (feof(handle)) {
			*eof = TRUE;
			//cout << "EOF reached " << endl;
		} else {
			*eof = FALSE;
		}
		fclose(handle);
		return bytesRead;
	}
}
void _convert_filename(char *filename) {
	int arrayLength = strlen(filename);
	int i;
	for (i = 0; i < arrayLength; i++) {
		if (filename[i] == '/') {
			filename[i] = '\\';
		}
	}
}

/** WinSound callback */
void mySoundProc(void *pSoundBuffer, long bufferLen) {
	if (sndMgr) {
		sndMgr->fillBuffer(pSoundBuffer, bufferLen);
	}
}

void _platform_init_sound(AudioManager *audioManager) {
	sndMgr = audioManager;
	winSound = new WinSound;
	if (winSound->start(mySoundProc)) {
		cout << "Windows Sound Started" << endl;
	}
}

void _platform_stop_sound() {
	cout << "Stopping Windows Sound" << endl;
	if (winSound) {
		winSound->stop();
	}
	winSound = NULL;
	cout << "Windows Sound Stopped" << endl;
}

void _platform_get_external_storage_dir_name(char* buf, S32 buflen) {
	getcwd(buf, buflen);
}

const char* _platform_get_path_separator() {
	return "\\";
}

BOOL32 _platform_path_exists(const char* path) {
	return (access(path, F_OK) != -1);
}

BOOL32 _platform_path_can_read(const char* path) {
	return (access(path, R_OK) != -1);
}

BOOL32 _platform_path_can_write(const char* path) {
	return (access(path, W_OK) != -1);
}

BOOL32 _platform_path_create(const char* path) {
	return (mkdir(path) == 0);
}

void _platform_play_vibration_effect(S32 effectId, F32 intensity) {
	//cout << "Playing vibration effect " << effectId << " at " << intensity << endl;
}

void _platform_start_vibration_effect(S32 effectId, F32 intensity) {
	//cout << "Starting vibration effect " << effectId << " at " << intensity << endl;
}

void _platform_stop_vibration_effect(S32 effectId) {
	//cout << "Stopping vibration effect " << effectId << endl;
}

void _platform_stop_all_vibration_effects() {
	//cout << "Stopping all vibration effects" << endl;
}

BOOL32 _platform_implements_soundpool() {
	return FALSE;
}

void _platform_init_audiomanagement(S32 streams){}
void _platform_release_audiomanagement(){}
void _platform_load_sound(const char* asset){}
S32 _platform_play_sound(const char* asset, F32 leftVol, F32 rightVol, S32 loops, F32 rate){ return -1; }
void _platform_stop_sound(S32 streamId){}
void _platform_stop_sound(const char* asset){}
void _platform_stop_all_sounds(){}
void _platform_unload_sound(const char *asset){}
void _platform_sound_set_loops(S32 streamId, S32 loops){}
void _platform_sound_set_volume(S32 streamId, F32 leftVol, F32 rightVol){}
void _platform_sound_set_rate(S32 streamId, F32 rate){}
S32 _platform_play_streaming_sound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate){ return -1; }
void _platform_stop_streaming_sound(const char *assetName){}
void _platform_show_keyboard(){}
void _platform_hide_keyboard(){}

void _platform_init_network() {
	WSADATA wsaData; // if this doesn't work
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed.\n");
		exit(1);
	}
}

void _platform_release_network() {
	WSACleanup();
}

void _platform_make_non_blocking(SOCKET socket) {
	u_long iMode = 1;
	//-------------------------
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled;
	// If iMode != 0, non-blocking mode is enabled.
	ioctlsocket(socket, FIONBIO, &iMode);
}

S32 _platform_get_socket_last_error() {
	return WSAGetLastError();
}

void _platform_exit() {
	// should link to boot's quit global
	extern BOOL32 quit;
	quit = TRUE;
}

void _platform_show_ad() {
	// Call out to your windows ad integration piece here
}
void _platform_hide_ad() {
	// Call out to your windows ad integration piece here
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
        if (af == AF_INET)
        {
                struct sockaddr_in in;
                memset(&in, 0, sizeof(in));
                in.sin_family = AF_INET;
                memcpy(&in.sin_addr, src, sizeof(struct in_addr));
                getnameinfo((struct sockaddr *)&in, sizeof(struct
sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
                return dst;
        }
        else if (af == AF_INET6)
        {
                struct sockaddr_in6 in;
                memset(&in, 0, sizeof(in));
                in.sin6_family = AF_INET6;
                memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
                getnameinfo((struct sockaddr *)&in, sizeof(struct
sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
                return dst;
        }
        return NULL;
}

#ifdef __MINGW32__
int inet_pton(int af, const char *src, void *dst)
{
        struct addrinfo hints, *res, *ressave;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = af;

        if (getaddrinfo(src, NULL, &hints, &res) != 0)
        {
               	cout << "Error - Couldn't resolve host " << src << endl;
                return -1;
        }

        ressave = res;

        while (res)
        {
                memcpy(dst, res->ai_addr, res->ai_addrlen);
                res = res->ai_next;
        }

        freeaddrinfo(ressave);
        return 0;
}
#endif

char** _platform_get_ifaddrs(int *count) {
	char** hostnames = new char*[1];
	hostnames[0] = new char[80];
	gethostname(hostnames[0], 80);
	struct hostent *phe = gethostbyname(hostnames[0]);
	if (phe == 0) {
		cout << "bad host lookup" << endl;
	}
	struct in_addr addr;
	memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
	hostnames[0] = inet_ntoa(addr);
	*count = 1;
	return hostnames;
}

void _platform_free_ifaddrs(char** ifaddrs, int count) {
	delete ifaddrs[0];
	delete [] ifaddrs;
}

#endif /* _WIN32 */
#endif /* WIN32GENERAL_CPP_ */
