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
// Name        : chromegeneral.cpp
// Description : Chrome platform general functions
// Usage       : See platformgeneral.h for usage
//============================================================================

#ifdef CHROME

#include "../platformgeneral.h"
#include "../../audio/AudioManager.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_graphics_3d.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_url_request_info.h"

using namespace BatteryTech;

extern PPB_Messaging* ppb_messaging_interface;
extern PPB_Var* ppb_var_interface;
extern PP_Instance g_instance;
extern PPB_Instance* ppb_instance_interface;
extern PPB_URLRequestInfo* ppb_urlrequestinfo_interface;
extern PPB_URLLoader* ppb_urlloader_interface;
extern PPB_Core* ppb_core_interface;

static struct PP_Var CStrToVar(const char* str) {
  if (ppb_var_interface != NULL) {
    return ppb_var_interface->VarFromUtf8(str, strlen(str));
  }
  return PP_MakeUndefined();
}

void _platform_log(const char* message) {
	if (ppb_messaging_interface) {
		// why is this double logging right now?
		ppb_messaging_interface->PostMessage(g_instance, CStrToVar(message));
	} else {
		printf(message);
		printf("\n");
	}
}

typedef void (*OpenCB)(void *dataPtr);
struct OpenRequest {
  PP_Resource loader_;
  PP_Resource request_;
  char* buf_;
  void* data_;
  S32* size_;
  int64_t avail_;
  OpenCB cb_;
  BOOL32* done;
};


void FreeRequest(OpenRequest* req) {
  if (req) {
    ppb_core_interface->ReleaseResource(req->request_);
    ppb_core_interface->ReleaseResource(req->loader_);
    free(req);
  }
}


static void URLPartialRead(void* user_data, int mode) {
  OpenRequest* req = (OpenRequest *) user_data;
  int64_t total;
  int32_t cnt;

  if (mode < 0) {
    free(req->buf_);
    req->cb_(NULL);
    FreeRequest(req);
    return;
  }

  req->avail_ += mode;
  total = *(req->size_) - req->avail_;

  cnt = (total > LONG_MAX) ? LONG_MAX : (int32_t) total;
  // If we still have more to do, re-issue the read.
  if (cnt > 0) {
    int32_t bytes = ppb_urlloader_interface->ReadResponseBody(req->loader_,
        (void *) &req->buf_[req->avail_], cnt,
        PP_MakeCompletionCallback(URLPartialRead, req));

    // If the reissue completes immediately, then process it.
    if (bytes != PP_OK_COMPLETIONPENDING) {
      URLPartialRead(user_data, bytes);
    }
    return;
  }

  // Nothing left, so signal complete.
  req->cb_(req);
  FreeRequest(req);
  printf("Loaded\n");
}


static void URLOpened(void* user_data, int mode) {
  OpenRequest* req = (OpenRequest *) user_data;

  int64_t cur, total;
  int32_t cnt;
  ppb_urlloader_interface->GetDownloadProgress(req->loader_, &cur, &total);

  // If we can't preallocate the buffer because the size is unknown, then
  // fail the load.
  if (total == -1) {
	  _platform_log("Error - Asset size is unknown.");
	*(req->size_) = 0;
	*(req->done) = TRUE;
    req->cb_(NULL);
    FreeRequest(req);
    return;
  }

  // Otherwise allocate a buffer with enough space for a terminating
  // NUL in case we need one.
  cnt = (total > LONG_MAX) ? LONG_MAX : (int32_t) total;
  req->buf_ = (char *) malloc(cnt + 1);
  req->buf_[cnt] = 0;
  *(req->size_) = cnt;
  _platform_log("Actually have a size here!");
  int32_t bytes = ppb_urlloader_interface->ReadResponseBody(req->loader_, req->buf_, cnt,
      PP_MakeCompletionCallback(URLPartialRead, req));

  // Usually we are pending.
  if (bytes == PP_OK_COMPLETIONPENDING) return;

  // But if we did complete the read, then dispatch the handler.
  URLPartialRead(req, bytes);
}

void LoadURL(PP_Instance inst, const char *url, OpenCB cb, void *data, S32 *size, BOOL32 *done) {
  OpenRequest* req = (OpenRequest*) malloc(sizeof(OpenRequest));
  memset(req, 0, sizeof(OpenRequest));

  req->loader_ = ppb_urlloader_interface->Create(inst);
  req->request_ = ppb_urlrequestinfo_interface->Create(inst);
  req->cb_ = cb;
  req->data_ = data;
  req->size_ = size;
  req->done = done;

  if (!req->loader_ || !req->request_) {
	  _platform_log("Error - No loader or no request");
	*size = 0;
	*(done) = TRUE;
    cb(NULL);
    FreeRequest(req);
    return;
  }

  ppb_urlrequestinfo_interface->SetProperty(req->request_,
      PP_URLREQUESTPROPERTY_URL, CStrToVar(url));
  ppb_urlrequestinfo_interface->SetProperty(req->request_,
      PP_URLREQUESTPROPERTY_METHOD, CStrToVar("GET"));
  ppb_urlrequestinfo_interface->SetProperty(req->request_,
      PP_URLREQUESTPROPERTY_RECORDDOWNLOADPROGRESS, PP_MakeBool(PP_TRUE));

  int val = ppb_urlloader_interface->Open(req->loader_, req->request_,
      PP_MakeCompletionCallback(URLOpened, req));

  if (val != PP_OK_COMPLETIONPENDING) {
	  _platform_log("Error - Completion not pending");
	*size = 0;
	*(done) = TRUE;
    cb(NULL);
    free(req);
  }
}

void Loaded(void* data) {
  OpenRequest *req = (OpenRequest *) data;
  if (req && req->buf_) {
    char **pptr = (char **) req->data_;
    *pptr = req->buf_;
	  _platform_log("Success!");
    *(req->done) = TRUE;
    return;
  }
  *(req->size_) = 0;
  *(req->done) = TRUE;
  _platform_log("Failed to load asset.");
}

unsigned char* _platform_load_internal_asset(const char *filename, S32 *size) {
	unsigned char *data;
	BOOL32 done = FALSE;
	char url[2048];
	*size = 0;
	sprintf(url, "assets/%s", filename);
	LoadURL(g_instance, url, Loaded, &data, size, &done);
	while (!done) {
		// wait for file... This won't work in Chrome because this and the IO are on same thread!!
		// will deadlock here.  TODO - solve!
	}
	if (*size == 0) {
		_platform_log("File not found");
		return NULL;
	}
	char buf[16384];
	sprintf(buf, "File %s is size %d", url, *size);
	_platform_log(buf);
	sprintf(buf, "Contents: %s", data);
	_platform_log(buf);
	return data;
}

void _platform_free_asset(unsigned char *ptr) {
}

S32 _platform_get_asset_length(const char *filename) {
	return 0;
}

S32 _platform_read_asset_chunk(const char *filename, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof) {
	return 0;
}

void _platform_init_sound(AudioManager *audioManager) {
}

void _platform_stop_sound() {
}

void _platform_get_external_storage_dir_name(char* buf, S32 buflen) {
}

void _platform_get_application_storage_dir_name(char* buf, S32 buflen) {
}

const char* _platform_get_path_separator() {
	return "/";
}

BOOL32 _platform_path_exists(const char* path) {
	return FALSE;
}

BOOL32 _platform_path_can_read(const char* path) {
	return FALSE;
}

BOOL32 _platform_path_can_write(const char* path) {
	return FALSE;
}

BOOL32 _platform_path_create(const char* path) {
	return FALSE;
}

void _platform_play_vibration_effect(S32 effectId, F32 intensity) {
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
}

void _platform_release_network() {
}

void _platform_make_non_blocking(SOCKET socket) {
}

S32 _platform_get_socket_last_error() {
	return 0;
}

void _platform_exit() {
}

void _platform_show_ad() {
}

void _platform_hide_ad() {
}

void _platform_hook(const char *hook, char *result, S32 resultLen) {
}

BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey) {
	return FALSE;
}

char** _platform_get_ifaddrs(int *count) {
	return NULL;
}

void _platform_free_ifaddrs(char** ifaddrs, int count) {
}

U64 _platform_get_time_nanos() {
	return 0;
}



#endif /* CHROME */
