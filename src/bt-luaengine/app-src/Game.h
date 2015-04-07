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

#ifndef GAME_H_
#define GAME_H_

#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/network/NetworkMessageListener.h>
#include <batterytech/math/Vector3.h>
#include "GameLuaBinder.h"
#include "GameConstants.h"
#include <batterytech/batterytech_entrypoints.h>

#if defined(BATTERYTECH_INCLUDE_BOX2D)
#include <bt-box2d/Dynamics/b2WorldCallbacks.h>
#define GAME_PARENTS , public b2ContactListener , public b2DestructionListener
#else
#define GAME_PARENTS
#endif

class LevelEditor;
class GameObject;
class World;
class LevelIO;
class TouchInputProcessor;
class GameContext;

namespace BatteryTech { class NetworkMessageFactory; }

using namespace BatteryTech;

class Game : public BTApplicationUpdater, NetworkMessageListener GAME_PARENTS {
public:
	Game(GameContext *context);
	virtual ~Game();
	// Implementing BTApplicationUpdater::update()
	virtual void update();
	// Implementing BTApplicationUpdater::onSuspend()
	virtual void onSuspend();
	void reset();
	void showError(const char* errorTitle, const char* errorMessage);
	void clearError();
	// called when a new network message has arrived.
	virtual void onNetworkMessage(NetworkMessage* message);
	// called when the network connection has disconnected.
	virtual void onNetworkDisconnected();
    void updatePhysics(F32 updateDelta, S32 substeps1, S32 substeps2);
    void updateGameObjects();
    void removeInactiveGameObjects();
	GameLuaBinder *luaBinder;
	TouchInputProcessor *touchInputProcessor;
	BOOL32 needsReset;
	BOOL32 isInError;
#ifdef BATTERYTECH_INCLUDE_BOX2D
    /* ----------------------- b2ContactListener Implementation ------------------------------ */
	/// Called when two fixtures begin to touch.
	virtual void BeginContact(b2Contact* contact);
	/// Called when two fixtures cease to touch.
	virtual void EndContact(b2Contact* contact);
	/// This is called after a contact is updated. This allows you to inspect a
	/// contact before it goes to the solver. If you are careful, you can modify the
	/// contact manifold (e.g. disable contact).
	/// A copy of the old manifold is provided so that you can detect changes.
	/// Note: this is called only for awake bodies.
	/// Note: this is called even when the number of contact points is zero.
	/// Note: this is not called for sensors.
	/// Note: if you set the number of contact points to zero, you will not
	/// get an EndContact callback. However, you may get a BeginContact callback
	/// the next step.
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	/// This lets you inspect a contact after the solver is finished. This is useful
	/// for inspecting impulses.
	/// Note: the contact manifold does not include time of impact impulses, which can be
	/// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
	/// in a separate data structure.
	/// Note: this is only called for contacts that are touching, solid, and awake.
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	/* ----------------------- end b2ContactListener Implementation ------------------------------ */
	/// Called when any joint is about to be destroyed due
	/// to the destruction of one of its attached bodies.
	virtual void SayGoodbye(b2Joint* joint);
	/// Called when any fixture is about to be destroyed due
	/// to the destruction of its parent body.
	virtual void SayGoodbye(b2Fixture* fixture);
#endif
private:
	GameContext *context;
	BOOL32 isFocused;
	BOOL32 initialized;
	void initializeLua();
	void updateState();
	void updateInput();
	void updateNetwork();
	World* getWorld();
};

#endif /* GAME_H_ */
