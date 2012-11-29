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
// Name        : Context.h
// Description : A central point of access for input and all service-level objects including rendering, sound, networking and vibration
// Usage       : Replace Game and World with your own implementations to hook into the platform
//============================================================================

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "primitives.h"
#include "util/HashTable.h"
#include "util/ManagedArray.h"
#include "util/Property.h"
#include "batterytech_entrypoints.h"

namespace BatteryTech {

	class UIManager;
	class MenuRenderer;
	class AudioManager;
	class NetworkManager;
	class VibrationManager;
	class GraphicsConfiguration;
	class RenderContext;
	class GLResourceManager;
	class QuadRenderer;
	class VideoManager;

	class Context {
	public:
		/**
		 * Represents the state of a pointer (mouse, touch or other)
		 */
		struct PointerState {
			BOOL32 isDown;
			S32 x;
			S32 y;
		};
		/**
		 * Represents the state of the accelerometer
		 */
		struct AccelerometerState {
			F32 x;
			F32 y;
			F32 z;
		};
		/**
		 * Represents the state of a keyboard key
		 */
		struct KeyState {
			BOOL32 isDown;
			U8 keyCode;
		};

		/**
		 * The different asset find functions, initially set by config text
		 */
		enum AssetFindFunction { ASSET_FIND_FUNCTION_INTERNAL, ASSET_FIND_FUNCTION_EXTERNAL, ASSET_FIND_FUNCTION_AUTO };

		Context(GraphicsConfiguration *gConfig);
		virtual ~Context();
		/**
		 * Array of pointer states to support multitouch.
		 * This will be MAX_POINTERS in length
		 */
		PointerState *pointerState;

		/**
		 * Only one accelerometer is supported and the current state of it is held here.
		 */
		AccelerometerState accelerometerState;

		/**
		 * For keyboard input, this is the last key the user pressed and is cleared by the UI library once consumed.
		 * As of Batterytech 1.0 - key events are not queued so if the user types faster than the FPS of the app, keys will be missed.
		 */
		U8 keyPressed;

		/**
		 * Special keys that a device may or may not have.  Query using _platform_has_special_key(BatteryTech::SpecialKey sKey)
		 */
		SpecialKey specialKeyPressed;

		/**
		 * For keyboard input, this is an array of the keys that are currently pressed down.  This array will be of MAX_KEYSTATES length.
		 */
		KeyState *keyState;

		/**
		 * Set to true if the UI is consuming the current pointers.
		 * Your app can use this to decide if it wants to ignore touches the UI is responding to.
		 * This is used instead of an event chain of consumption pattern.
		 */
		BOOL32 isUIConsumingPointers;

		/**
		 * Set to true if the UI is consuming the current input keys.
		 * Your app can use this to decide if it wants to ignore keys the UI is responding to.
		 * This is used instead of an event chain of consumption pattern.
		 */
		BOOL32 isUIConsumingKeys;

		/**
		 * The time in seconds that has passed between the last update and this update.
		 * Use this for timing animations, physics and anything else time-sensitive.
		 */
		F32 tickDelta;

		/**
		 * Display the current frames per second?
		 */
		BOOL32 showFPS;

		/**
		 * Set to true if the graphics context was lost and there is a new graphics context.
		 * If this is set to true, you must reload all VRAM objects (textures, FBOs, VBOs, etc) and reset the GL states before continuing.
		 */
		BOOL32 newGraphicsContext;

		/**
		 * Use this for playing sound effects and music.
		 */
		AudioManager *audioManager;

		/**
		 * Use this for establishing network connections, or replace with your own implementation.
		 */
		NetworkManager *networkManager;

		/**
		 * Use this for playing vibration effects.
		 * Platform-specific implementation is required (as of batterytech 1.0, except for IOS which only has one vibe)
		 */
		VibrationManager *vibrationManager;

		/**
		 * This is the default Batterytech UI Renderer.
		 */
		MenuRenderer *menuRenderer;

		/**
		 * This holds the configuration for the current graphics system.
		 */
		GraphicsConfiguration *gConfig;

		/**
		 * Use this to add and show menus and manipulate the menu stack.
		 */
		UIManager *uiManager;

		/**
		 * This is the current rendering context.  Among other things, it should be used to track matrices and states.
		 */
		RenderContext *renderContext;

		/**
		 * This manages shared GL-related resources such as Textures, TextureRegions, Shaders, etc
		 */
		GLResourceManager *glResourceManager;

		/** \brief A common QuadRenderer
		 *
		 */
		QuadRenderer *quadRenderer;

		/**
		 * Async callback data (see btCallback())
		 */
		char callbackData[1024];

		/**
		 * If there was a callback and there is callback data ready
		 */
		BOOL32 callbackDataReady;

		VideoManager *videoManager;

		BTApplicationRenderer *appRenderer;

		BTApplicationUpdater *appUpdater;

		StrHashTable<Property*> *appProperties;

		AssetFindFunction assetFindFunction;
	private:
		void loadAppProperties();
	};
}

#endif /* CONTEXT_H_ */
