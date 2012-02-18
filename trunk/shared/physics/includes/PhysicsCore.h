/**
 * @file	PhysicsCore.h
 * @brief 	Contains the physics world and methods relating to it (currently there are not many such methods)
 */
#ifndef __PhysicsCore_h_
#define __PhysicsCore_h_

#include "stdafx.h"
#include "SharedIncludes.h"
#include "boost\lexical_cast.hpp"

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
    void createFloorPlane (Ogre::SceneNode *arenaNode);
    void createWallPlanes (void);
    void newBox(Ogre::SceneNode *node, Ogre::Vector3 position, Ogre::Vector3 size, Ogre::Vector3 cameraDirectionNormalised, float mass);
    OgreBulletDynamics::RaycastVehicle *newCar(Ogre::Vector3 carPosition,
                                               Ogre::Vector3 chassisShift,
                                               Ogre::SceneNode *carNode,
                                               Ogre::SceneNode *wheelNodeFrontLeft,
                                               Ogre::SceneNode *wheelNodeFrontRight,
                                               Ogre::SceneNode *wheelNodeRearLeft,
                                               Ogre::SceneNode *wheelNodeRearRight);
    void addCube(const Ogre::String instanceName, const Ogre::Vector3 pos, const Ogre::Quaternion q, const Ogre::Vector3 size,
                 const Ogre::Real bodyRestitution, const Ogre::Real bodyFriction, const Ogre::Real bodyMass);
    void stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real fixedTimestep);

    OgreBulletDynamics::DynamicsWorld *mWorld; // Collisions object

private:
    static void preTickCallback(btDynamicsWorld *world, btScalar timeStep);
    static void postTickCallback(btDynamicsWorld *world, btScalar timeStep);

    PlayerCollisions* mPlayerCollisions;
    std::deque<OgreBulletDynamics::RigidBody *>        mBodies;
    std::deque<OgreBulletCollisions::CollisionShape *> mShapes;
    Ogre::Vector3 mBulletGravity;
    Ogre::AxisAlignedBox mBulletAlignedBox;
    OgreBulletCollisions::DebugDrawer *debugDrawer;
    int mNumEntitiesInstanced;
};

#endif // #ifndef __PhysicsCore_h_

