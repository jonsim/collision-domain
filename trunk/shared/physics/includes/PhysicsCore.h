/**
 * @file	PhysicsCore.h
 * @brief 	Contains the physics world and methods relating to it (currently there are not many such methods)
 */
#ifndef __PhysicsCore_h_
#define __PhysicsCore_h_

#include "stdafx.h"
#include "PlayerCollisions.h"


#ifdef _WIN32
	#include "boost\lexical_cast.hpp"
#else
	#include "boost/lexical_cast.hpp"
#endif

// This is used for physics collision masks
enum QueryFlags
{
    // Raycast car must be 1 as the bullet wheel raycasts are broken and assume this collision mask (1)
    COL_NOTHING    = 0,
    COL_CAR        = 1<<0,
    COL_ARENA      = 1<<1,
    COL_POWERUP    = 1<<2
};

class PhysicsCore
{
public:
    PhysicsCore (void);
    virtual ~PhysicsCore (void);
	static void auto_scale_scenenode (Ogre::SceneNode* n);
    int getUniqueEntityID (void);
    btRigidBody* createArenaBody( Ogre::SceneNode *targetNode, ArenaID aid );

    void stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real fixedTimestep);
    
    btDynamicsWorld* getWorld() { return mBulletWorld; }

    void createCollisionShapes();
    btCollisionShape *getCollisionShape( PHYS_SHAPE shape ) { return mShapes[shape]; }
    void setCollisionShape( PHYS_SHAPE shape, btCollisionShape *btshape ) { mShapes[shape] = btshape; }
    void addRigidBody( btRigidBody *body, short colGroup, short colMask );
    bool removeBody( btRigidBody *body );
    void clearWorld();

    bool singleObjectRaytest(const btVector3& rayFrom, const btVector3& rayTo, btVector3& worldNormal, btVector3& worldHitPoint);

    //OgreBulletDynamics::DynamicsWorld *mWorld; // Collisions object

    PlayerCollisions* mPlayerCollisions;


private:
    static void preTickCallback(btDynamicsWorld *world, btScalar timeStep);
    static void postTickCallback(btDynamicsWorld *world, btScalar timeStep);

    //std::deque<OgreBulletDynamics::RigidBody *>        mBodies;
    //std::deque<OgreBulletCollisions::CollisionShape *> mShapes;
    Ogre::Vector3 mBulletGravity;
    Ogre::AxisAlignedBox mBulletAlignedBox;
    int mNumEntitiesInstanced;

    btDynamicsWorld                     *mBulletWorld;
    btCollisionWorld                   *mCollisionWorld;
    btAxisSweep3                        *mBroadphase;
    btDefaultCollisionConfiguration     *mCollisionConfig;
    btCollisionDispatcher               *mDispatcher;
    btSequentialImpulseConstraintSolver *mSolver;

    std::deque<btRigidBody*>             mBodies;
    //std::deque<btCollisionShape*>      mShapes;
    btCollisionShape                    *mShapes[PHYS_SHAPE_COUNT];

    BtOgre::DebugDrawer                 *dbgDraw;

};

#endif // #ifndef __PhysicsCore_h_

