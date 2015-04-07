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
// Name        : iosgeneral.mm
// Description : iOS platform general functions
// Usage       : See platformgeneral.h for usage
//============================================================================

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

#include "../platformgeneral.h"
#import "../../batterytech.h"
#include "batterytechKeyboardDelegate.h"
#include <stdlib.h>
#include <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../../util/strx.h"
#include <mach/mach.h>
#include <mach/mach_time.h>

#import "batterytechViewController.h"

#define ASSETS_DIR "assets/"

using namespace BatteryTech;

AudioManager *_iosSndMgr;
@class btInputView;
btInputView *myTextView;
extern batterytechViewController *btViewController;

static const char* getFilePathForAsset(const char *assetName) {
	char *lastDot = NULL;
	char *lastSep = NULL;
	// assetName will be "dir1/dir2/my_file.ogg"
	char path[1024]; // path should be "/assets/dir1/dir2"
	char extension[10]; // extension should be "ogg"
	char filename[1024]; // filename should be "my_file"
	lastSep = strrchr(assetName, '/');
	if (lastSep) {
		strcpy(filename, lastSep + 1);
	} else {
		strcpy(filename, assetName);
	}
	lastDot = strrchr(filename, '.');
	if (lastDot) {
		int lastDotPos = lastDot - filename;
		int extLength = strlen(filename) - lastDotPos - 1;
		strcpy(path, ASSETS_DIR);
		if (lastSep) {
			strncat(path, assetName, lastSep - assetName);
			path[strlen(ASSETS_DIR) + (lastSep - assetName)] = '\0';
		} else {
			path[strlen(ASSETS_DIR)] = '\0';
		}
		strncpy(extension, filename + lastDotPos + 1, extLength);
		extension[extLength] = '\0';
		// now that we've pulled the extension off, we can chop the filename to become the resource name.
		filename[lastDot - filename] = '\0';
		NSString *resourceName = [NSString stringWithCString:filename encoding: NSUTF8StringEncoding];
		NSString *extString = [NSString stringWithCString:extension encoding: NSUTF8StringEncoding];
		NSString *pathName = [NSString stringWithCString:path encoding: NSUTF8StringEncoding];
		__block NSString *filePath = [[NSBundle mainBundle] pathForResource:resourceName ofType:extString inDirectory:pathName];
        if (! filePath) {
            NSLog(@"Error finding asset %@", resourceName);
        }
		const char *filePathCString = [filePath UTF8String];
		return filePathCString;
	}
	return NULL;
}

void _platform_log(const char* message) {
	NSLog(@"%s", message);
}

unsigned char* _platform_load_internal_asset(const char *assetName, int *size) {
	const char *filePathCString = getFilePathForAsset(assetName);
	if (filePathCString) {
		FILE *handle;
		unsigned char *data = 0;
		handle = fopen(filePathCString, "rb");
		if (!handle) {
			NSLog(@"No File Handle");		
		}
		fseek(handle, 0L, SEEK_END);
		*size = ftell(handle);
		//fseek(handle, 0L, SEEK_SET);
		rewind(handle);
		data = (unsigned char*) malloc(sizeof(unsigned char) * *size);
		if (data) {
			fread(data, sizeof(unsigned char), *size, handle);
			// int bytesRead = fread(data, sizeof(unsigned char), *size, handle);
			// cout << "malloc success, " << bytesRead << " bytes read of " << *size << endl;
		}
		int error = ferror(handle);
		if (error) {
			//cout << "IO error " << error << endl;
		}
		if (feof(handle)) {
			//cout << "EOF reached " << endl;
		}
		fclose(handle);
		return data;
	}
	return 0;
}

void _platform_free_asset(unsigned char *ptr) {
	if (ptr) {
		free(ptr);
	}
}

S32 _platform_get_asset_length(const char *assetName) {
	int size = 0;
	const char *filePathCString = getFilePathForAsset(assetName);
	if (filePathCString) {
		FILE *handle;
		handle = fopen(filePathCString, "rb");
		if (!handle) {
			NSLog(@"No File Handle");		
		}
		fseek(handle, 0L, SEEK_END);
		size = ftell(handle);
		fclose(handle);
	}
	return size;
}

S32 _platform_read_asset_chunk(const char *assetName, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof) {
	const char *filePathCString = getFilePathForAsset(assetName);
	if (filePathCString) {
		FILE *handle;
		handle = fopen(filePathCString, "rb");
		if (!handle) {
			NSLog(@"read asset chunk No File Handle");		
		}
		fseek(handle, offset, SEEK_SET);
		int bytesRead = fread(buffer, sizeof(unsigned char), bufferLength, handle);
		int error = ferror(handle);
		if (error) {
            NSLog(@"read asset chunk IO error %d", error);
			//cout << "IO error " << error << endl;
		}
		if (feof(handle)) {
			*eof = TRUE;
			//cout << "EOF reached " << endl;
		} else {
			*eof = FALSE;
		}
		fclose(handle);
		return bytesRead;
	} else {
        NSLog(@"read asset chunk coudln't find path for %s", assetName);
    }
	return 0;
}

void _platform_init_sound(AudioManager *audioManager) {
	_iosSndMgr = audioManager;
}

void _platform_stop_sound() {
}

void _platform_get_external_storage_dir_name(char* buf, S32 buflen) {
	// iOS Apps are always sandboxed.  No need to create specific storage name dir
	// Documents dir is exposed to user and backed up.
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	const char *docDirCString = [documentsDirectory UTF8String];
	strcpy(buf, docDirCString);
	buf[strlen(docDirCString)] = '\0';
 }

void _platform_get_application_storage_dir_name(char* buf, S32 buflen) {
	// iOS Apps are always sandboxed.  No need to create specific storage name dir
	// Application Support dir AKA <sandbox>/Library is not exposed to user but (with exception of Cache) is backed up.
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	const char *docDirCString = [documentsDirectory UTF8String];
	strcpy(buf, docDirCString);
	buf[strlen(docDirCString)] = '\0';
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
	return (mkdir(path, (mode_t)0755) == 0);
}

void _platform_play_vibration_effect(S32 effectId, F32 intensity) {
    if (effectId == 0) {
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
    }
}

void _platform_start_vibration_effect(S32 effectId, F32 intensity) {
}

void _platform_stop_vibration_effect(S32 effectId) {
}

void _platform_stop_all_vibration_effects() {
}

BOOL32 _platform_implements_soundpool() {
	return FALSE;
}

void _platform_init_audiomanagement(S32 streams) {
}

void _platform_release_audiomanagement() {
}

void _platform_load_sound(const char* asset) {
}

S32 _platform_play_sound(const char* asset, F32 leftVol, F32 rightVol, S32 loops, F32 rate) {
	return 0;
}

void _platform_stop_sound(S32 streamId) {
}

void _platform_stop_sound(const char* asset) {
}

void _platform_stop_all_sounds() {
}

void _platform_unload_sound(const char *asset) {
}

void _platform_sound_set_loops(S32 streamId, S32 loops){}
void _platform_sound_set_volume(S32 streamId, F32 leftVol, F32 rightVol){}
void _platform_sound_set_rate(S32 streamId, F32 rate){}
S32 _platform_play_streaming_sound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate){ return 0; }
void _platform_stop_streaming_sound(const char *assetName){}

@interface btInputView : UIView<UIKeyInput>
@end

@implementation btInputView

-(BOOL)canBecomeFirstResponder {
    return TRUE;
}
-(void)deleteBackward {
    @synchronized([self superview]) {
        NSLog(@"backspace pressed");
        btKeyPressed(8, BatteryTech::SKEY_NULL);
    }
}

-(BOOL)hasText {
    return TRUE;
}

-(void)insertText:(NSString *)text {
    @synchronized([self superview]) {
        NSLog(@"%@ key pressed", text);
        btKeyPressed([text characterAtIndex:0], BatteryTech::SKEY_NULL);
    }
}

@end


void _platform_show_keyboard() {
    dispatch_async( dispatch_get_main_queue(), ^{
        NSLog(@"Showing keyboard");
        extern UIView *batterytechRootView;
        // create hidden textview and set as first responder
        myTextView = [[btInputView alloc] initWithFrame:CGRectMake(0, 0,1,1)];
        [batterytechRootView addSubview:myTextView];
        [myTextView becomeFirstResponder];
    });
}

void _platform_hide_keyboard() {
    dispatch_async( dispatch_get_main_queue(), ^{
        NSLog(@"Hiding keyboard");
        // resign responder from textview
        // release textview and null out
        if (myTextView) {
            [myTextView resignFirstResponder];
            [myTextView removeFromSuperview]; 
            [myTextView release];
            myTextView = NULL;
        }
        if (FALSE) {
            // uncomment this if you need failsafe keyboard removal
            UIWindow* tempWindow;
            for(int c = 0; c < [[[UIApplication sharedApplication] windows] count]; c ++)
            {
                tempWindow = [[[UIApplication sharedApplication] windows] objectAtIndex:c];
                for(int i = 0; i < [tempWindow.subviews count]; i++)
                {
                    UIView *keyboard = [tempWindow.subviews objectAtIndex:i];
                    if([[keyboard description] hasPrefix:@"<UIKeyboard"] == YES
                       || [[keyboard description] hasPrefix:@"<UIPeripheralHostView"] == YES 
                       || [[keyboard description] hasPrefix:@"<UISnap"] == YES ){	
                        [keyboard setHidden:TRUE];
                    }
                }
            }
        }
    });
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
	//__android_log_print(ANDROID_LOG_DEBUG, "Batterytech", "Getting host names");
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

void _platform_exit() {
	// Apple recommends against forcefully quitting an app.  If you should choose to quit forcefully,
	// exit(0);
}

void _platform_show_ad() {
	// Call out to your ios ad integration piece here
    dispatch_async( dispatch_get_main_queue(), ^{
        [btViewController showAd];
    });
}

void _platform_hide_ad() {
	// Call out to your ios ad integration piece here
    dispatch_async( dispatch_get_main_queue(), ^{
        [btViewController hideAd];
    });
}

void _platform_hook(const char *hook, char *result, S32 resultLen) {
	// Handle custom hooks here
    [btViewController hook:hook withResult:result withResultLen:resultLen];
}

BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey) {
	return FALSE;
}

U64 _platform_get_time_nanos() {
	static mach_timebase_info_data_t sTimebaseInfo;
	uint64_t time = mach_absolute_time();
	uint64_t nanos;
	// If this is the first time we've run, get the timebase.
	// We can use denom == 0 to indicate that sTimebaseInfo is
	// uninitialised because it makes no sense to have a zero
	// denominator is a fraction.
	if ( sTimebaseInfo.denom == 0 ) {
		(void) mach_timebase_info(&sTimebaseInfo);
	}
	// Do the maths.  We hope that the multiplication doesn't
	// overflow; the price you pay for working in fixed point.
	nanos = time * sTimebaseInfo.numer / sTimebaseInfo.denom;
	return nanos;
}

#endif
