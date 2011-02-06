/*
 * Game.h
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#ifndef GAME_H_
#define GAME_H_

#include <batterytech/primitives.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/network/NetworkMessageListener.h>

class Context;
class LevelEditor;
class GameObject;
class World;
class TouchInputProcessor;
class LevelIO;
class NetworkMessageFactory;

class Game : b2ContactListener, b2DestructionListener, NetworkMessageListener {
public:
	Game(Context *context);
	virtual ~Game();
	virtual void update();
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
	// called when a new network message has arrived.
	virtual void onNetworkMessage(NetworkMessage* message);
	// called when the network connection has disconnected.
	virtual void onNetworkDisconnected();
private:
	void loadPreferences();
	Context *context;
	BOOL32 isFocused;
	BOOL32 initialized;
	TouchInputProcessor *touchInputProcessor;
	void loadLevel();
	void initializeNewGame();
	void updateState();
	void updatePhysics();
	void updateGameObjects();
	void updateLevel();
	void updateGameObjectArray(ManagedArray<GameObject> *objects);
	void updateInput();
	void updateNetwork();
	F32 screenToWorldX(S32 screenX);
	F32 screenToWorldY(S32 screenY);
	World* getWorld();
};

#endif /* GAME_H_ */
