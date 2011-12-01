
#ifndef __PhysicsCore_h_
#define __PhysicsCore_h_

#include "stdafx.h"

// This is used for physics integration with ogre. (And neat housekeeping)
enum QueryFlags
{
    ANY_QUERY_MASK             = 1<<0,
    RAGDOLL_QUERY_MASK         = 1<<1,
    GEOMETRY_QUERY_MASK        = 1<<2,
    VEHICLE_QUERY_MASK         = 1<<3,
    STATIC_GEOMETRY_QUERY_MASK = 1<<4
};

class PhysicsCore
{
public:
    OgreBulletDynamics::DynamicsWorld *mWorld; // Collisions object

    PhysicsCore(Ogre::SceneManager* sceneMgr);
    virtual ~PhysicsCore(void);
    int getUniqueEntityID(void);
    void newPlane();
    void newBox(Ogre::SceneNode *node, Ogre::Vector3 position, Ogre::Vector3 size, Ogre::Vector3 cameraDirectionNormalised);
    OgreBulletDynamics::RaycastVehicle *newCar(Ogre::Vector3 carPosition,
                                               Ogre::Vector3 chassisShift,
                                               Ogre::SceneNode *carNode,
                                               Ogre::SceneNode *wheelNodeFrontLeft,
                                               Ogre::SceneNode *wheelNodeFrontRight,
                                               Ogre::SceneNode *wheelNodeRearLeft,
                                               Ogre::SceneNode *wheelNodeRearRight);

private:
    Ogre::SceneManager* mSceneMgr; // Graphics object
    std::deque<OgreBulletDynamics::RigidBody *>        mBodies;
    std::deque<OgreBulletCollisions::CollisionShape *> mShapes;
    Ogre::Vector3 mBulletGravity;
    Ogre::AxisAlignedBox mBulletAlignedBox;
    OgreBulletCollisions::DebugDrawer *debugDrawer;
    int mNumEntitiesInstanced;
};

#endif // #ifndef __PhysicsCore_h_
