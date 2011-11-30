
#ifndef __CDPhysicsCore_h_
#define __CDPhysicsCore_h_

#include "OgreIncludes.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "Shapes/OgreBulletCollisionsCompoundShape.h"

class CDPhysicsCore
{
public:
    OgreBulletDynamics::DynamicsWorld *mWorld; // Collisions object

    CDPhysicsCore(Ogre::SceneManager* sceneMgr);
    virtual ~CDPhysicsCore(void);
	int getUniqueEntityID(void);
	void CDPhysicsCore::newPlane();
	void CDPhysicsCore::newBox(Ogre::SceneNode *node, Ogre::Vector3 position, Ogre::Vector3 size, Ogre::Vector3 cameraDirectionNormalised);
	void newCar(Ogre::Vector3 carPosition,
				Ogre::Vector3 chassisShift,
				Ogre::SceneNode *carNode,
				Ogre::SceneNode *wheelNode0,
				Ogre::SceneNode *wheelNode1,
				Ogre::SceneNode *wheelNode2,
				Ogre::SceneNode *wheelNode3);

private:
	Ogre::SceneManager* mSceneMgr; // Graphics object
    std::deque<OgreBulletDynamics::RigidBody *>        mBodies;
    std::deque<OgreBulletCollisions::CollisionShape *> mShapes;
	Ogre::Vector3 mBulletGravity;
	Ogre::AxisAlignedBox mBulletAlignedBox;
    OgreBulletCollisions::DebugDrawer *debugDrawer;
    int mNumEntitiesInstanced;
	Ogre::Real mBulletMoveSpeed;
};

#endif // #ifndef __CDPhysicsCore_h_
