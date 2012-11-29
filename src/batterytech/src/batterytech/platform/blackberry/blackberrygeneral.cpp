/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : androidgeneral.cpp
// Description : Android platform general functions
// Usage       : See platformgeneral.h for usage
//============================================================================

#if defined(__QNXNTO__)

#include "../platformgeneral.h"
#include "blackberrygeneral.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../primitives.h"
#include "../../audio/AudioManager.h"
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <string.h>
#include <iostream.h>
#include "../../batterytech.h"
#include "../../Context.h"
#include <bps/virtualkeyboard.h>
#include "BlackberryAudioGW.h"
#include <bps/paymentservice.h>

using namespace BatteryTech;
using namespace std;

BlackberryAudioGW *bbAudioGW = NULL;

void _convert_filename(char *filename);

void _platform_log(const char* message) {
	cout << message << endl;
}

unsigned char* _platform_load_internal_asset(const char *filename, S32 *size) {
	//char *myFilename;
	//strcpy(myFilename, filename);
	//_convert_filename(myFilename);
	char myFilename[255];
	strcpy(myFilename, "app/native/assets/");
	strcat(myFilename, filename);
	_platform_convert_path(myFilename, myFilename);
	//cout << "trying " << myFilename << endl;
	FILE *handle;
	unsigned char *data = 0;
	handle = fopen(myFilename, "rb");
	if (!handle) {
		cout << "File not found: " << myFilename << endl;
		return 0;
	}
	fseek(handle, 0L, SEEK_END);
	*size = ftell(handle);
	//fseek(handle, 0L, SEEK_SET);
	rewind(handle);
	data = (unsigned char*) malloc(sizeof(unsigned char) * *size);
	if (data) {
		int bytesRead = fread(data, sizeof(unsigned char), *size, handle);
		// cout << "malloc success, " << bytesRead << " bytes read of " << *size << endl;
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
	strcpy(myFilename, "app/native/assets/");
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
	strcpy(myFilename, "app/native/assets/");
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
		if (filename[i] == '\\') {
			filename[i] = '/';
		}
	}
}

void _platform_init_sound(AudioManager *audioManager) {
	bbAudioGW = new BlackberryAudioGW(audioManager);
	bbAudioGW->init();
}

void _platform_stop_sound() {
	bbAudioGW->release();
	delete bbAudioGW;
	bbAudioGW = NULL;
}

void _platform_get_external_storage_dir_name(char* buf, S32 buflen) {
	strcpy(buf, "data/");
	buf[5] = '\0';
	//strcat(buf, btGetContext()->appProperties->get("storage_dir")->getValue());
}

void _platform_get_application_storage_dir_name(char* buf, S32 buflen) {
	strcpy(buf, "data/");
	buf[5] = '\0';
	//strcat(buf, btGetContext()->appProperties->get("storage_dir")->getValue());
}

const char* _platform_get_path_separator() {
	return "/";
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
	return (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
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

void _platform_show_keyboard() {
	virtualkeyboard_show();
}

void _platform_hide_keyboard() {
	virtualkeyboard_hide();
}

void _platform_exit() {
	// quit = TRUE;
}

BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey) {
	return FALSE;
}


void _platform_init_network() {}
void _platform_release_network() {}

void _platform_make_non_blocking(SOCKET socket) {
	fcntl(socket, F_SETFL, O_NONBLOCK);
}

S32 _platform_get_socket_last_error() {
	return errno;
}

char** _platform_get_ifaddrs(int *count) {
	_platform_log("Getting host names");
	*count = 0;
	char** hostnames = NULL;
	struct ifreq *ifr;
	struct ifconf ifc;
	int s, i;
	int numif;

	// find number of interfaces.
	memset(&ifc, 0, sizeof(ifc));
	ifc.ifc_ifcu.ifcu_req = NULL;
	ifc.ifc_len = 0;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return hostnames;
	}
	if (ioctl(s, SIOCGIFCONF, &ifc) < 0) {
		return hostnames;
	}
	if ((ifr = (struct ifreq*) malloc(ifc.ifc_len)) == NULL) {
		return hostnames;
	}
	ifc.ifc_ifcu.ifcu_req = ifr;
	if (ioctl(s, SIOCGIFCONF, &ifc) < 0) {
		return hostnames;
	}
	close(s);
	numif = ifc.ifc_len / sizeof(struct ifreq);
	*count = numif;
	hostnames = new char*[numif];
	for (i = 0; i < numif; i++) {
		struct ifreq *r = &ifr[i];
		struct sockaddr_in *sin = (struct sockaddr_in *)&r->ifr_addr;
		hostnames[i] = new char[80];
		strcpy(hostnames[i], inet_ntoa(sin->sin_addr));
	}
	free(ifr);
	return hostnames;
}

void _platform_free_ifaddrs(char** ifaddrs, int count) {
	for (S32 i = 0; i < count; i++) {
		delete ifaddrs[i];
	}
	delete [] ifaddrs;
}

// Returns a time in nanoseconds, suitable for high resolution timing and profiling
U64 _platform_get_time_nanos() {
   timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}

#endif /* bb qnx */
