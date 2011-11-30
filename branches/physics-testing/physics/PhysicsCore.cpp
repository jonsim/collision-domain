

#include "CDPhysicsCore.h"


CDPhysicsCore::CDPhysicsCore(Ogre::SceneManager* sceneMgr)
{
	mSceneMgr = sceneMgr;

	mBulletGravity = Ogre::Vector3(0,-9.81,0);
	mBulletAlignedBox = Ogre::AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000), Ogre::Vector3(10000,  10000,  10000));
	mBulletMoveSpeed = 50;   // defined in ExampleFrameListener
    mNumEntitiesInstanced = 0; // how many shapes are created

	// Start Bullet
    mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, mBulletAlignedBox, mBulletGravity);
    // add Debug info display tool
    debugDrawer = new OgreBulletCollisions::DebugDrawer();
    debugDrawer->setDrawWireframe(true);   // we want to see the Bullet containers
    mWorld->setDebugDrawer(debugDrawer);
    mWorld->setShowDebugShapes(true);      // enable it if you want to see the Bullet containers
    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject(static_cast <Ogre::SimpleRenderable *> (debugDrawer));
}


CDPhysicsCore::~CDPhysicsCore(void)
{
	// OgreBullet physic delete - RigidBodies
	std::deque<OgreBulletDynamics::RigidBody *>::iterator itBody = mBodies.begin();
	while (mBodies.end() != itBody)
	{   
		delete *itBody;
		++itBody;
	}
	// OgreBullet physic delete - Shapes
	std::deque<OgreBulletCollisions::CollisionShape *>::iterator itShape = mShapes.begin();
	while (mShapes.end() != itShape)
	{   
		delete *itShape;
		++itShape;
	}
	mBodies.clear();
	mShapes.clear();
	delete mWorld->getDebugDrawer();
	mWorld->setDebugDrawer(0);
	delete mWorld;
}


/**	@brief mNumEntitiesInstances should only ever be modified through this method! (Potential crashes otherwise)
	@return an int which has never before been used (in an entity name) */
int CDPhysicsCore::getUniqueEntityID()
{
	mNumEntitiesInstanced++;
	return mNumEntitiesInstanced;
}


void CDPhysicsCore::newPlane()
{
	OgreBulletCollisions::CollisionShape *Shape;
    Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 0); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody = new OgreBulletDynamics::RigidBody(
            "BasePlane",
            mWorld);
    defaultPlaneBody->setStaticShape(Shape, 0.1, 0.8); // (shape, restitution, friction)
    // push the created objects to the deques
    mShapes.push_back(Shape);
    mBodies.push_back(defaultPlaneBody);
}


void CDPhysicsCore::newBox(Ogre::SceneNode *node, Ogre::Vector3 position, Ogre::Vector3 size, Ogre::Vector3 cameraDirectionNormalised)
{
		size *= 0.05f;                  // don't forget to scale down the Bullet-box too
		// after that create the Bullet shape with the calculated size
		OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
		// and the Bullet rigid body
		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
			"defaultBoxRigid" + Ogre::StringConverter::toString(getUniqueEntityID()),
			mWorld);
		defaultBody->setShape(node,
							  sceneBoxShape,
							  0.6f,         // dynamic body restitution
							  0.6f,         // dynamic body friction
							  1.0f,          // dynamic bodymass
							  position,      // starting position of the box
							  Ogre::Quaternion(0,0,0,1));// orientation of the box
		defaultBody->setLinearVelocity(cameraDirectionNormalised * 7.0f ); // shooting speed
			// push the created objects to the deques
		mShapes.push_back(sceneBoxShape);
		mBodies.push_back(defaultBody);
}


/** @brief There is still a lot of hardcoded stuff in here, which will vary depending on mesh size and initial rotation
*/
void CDPhysicsCore::newCar(
	Ogre::Vector3 carPosition,
	Ogre::Vector3 chassisShift,
	Ogre::SceneNode *carNode,
	Ogre::SceneNode *wheelNode0,
	Ogre::SceneNode *wheelNode1,
	Ogre::SceneNode *wheelNode2,
	Ogre::SceneNode *wheelNode3)
{
	OgreBulletDynamics::WheeledRigidBody        *mCarChassis;
	OgreBulletDynamics::VehicleTuning	        *mTuning;
	OgreBulletDynamics::VehicleRayCaster	    *mVehicleRayCaster;
	OgreBulletDynamics::RaycastVehicle	        *mVehicle;
			
	static float	gSuspensionStiffness = 20.f;
	static float	gSuspensionDamping = 2.3f;
	static float	gSuspensionCompression = 4.4f;
	static float	gRollInfluence = 0.1f;//1.0f;
	static float	gSuspensionRestLength = 0.6;
	static float	gMaxSuspensionTravelCm = 500.0;
	static float	gFrictionSlip = 10.5;
			
	static float	gWheelRadius = 0.5f;
	static float	gWheelWidth = 0.4f;
	static float	gWheelFriction = 1e30f;//1000;//1e30f;
	float connectionHeight = 0.7f;

	OgreBulletCollisions::BoxCollisionShape* chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1.f,0.75f,2.1f));
	OgreBulletCollisions::CompoundCollisionShape* compound = new OgreBulletCollisions::CompoundCollisionShape();

	compound->addChildShape(chassisShape, chassisShift);

	mCarChassis = new OgreBulletDynamics::WheeledRigidBody("carChassis" + Ogre::StringConverter::toString(getUniqueEntityID()), mWorld); // name given here needs to be unique to have more than one in the scene

	/*mShapes.push_back(chassisShape);
	mShapes.push_back(compound);
	mBodies.push_back(mCarChassis);*/

	mCarChassis->setShape (carNode, compound, 0.6, 0.6, 800, carPosition, Ogre::Quaternion::IDENTITY);
	mCarChassis->setDamping(0.2, 0.2);

	mCarChassis->disableDeactivation ();
	mTuning = new OgreBulletDynamics::VehicleTuning(
		gSuspensionStiffness,
		gSuspensionCompression,
		gSuspensionDamping,
		gMaxSuspensionTravelCm,
		gFrictionSlip);

	mVehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(mWorld);
	mVehicle = new OgreBulletDynamics::RaycastVehicle(mCarChassis, mTuning, mVehicleRayCaster);
	
	{
		// This line is needed otherwise the model appears wrongly rotated.
		mVehicle->setCoordinateSystem(0, 1, 2); // rightIndex, upIndex, forwardIndex
		
		Ogre::Vector3 wheelDirectionCS0(0,-1,0);
		Ogre::Vector3 wheelAxleCS(-1,0,0);

		{
			#define CUBE_HALF_EXTENTS 1
			bool isFrontWheel = true;

			// Wheel 1
			Ogre::Vector3 connectionPointCS0 (
				CUBE_HALF_EXTENTS-(0.3*gWheelWidth),
				connectionHeight,
				2*CUBE_HALF_EXTENTS-gWheelRadius);
			mVehicle->addWheel(
				wheelNode0,
				connectionPointCS0,
				wheelDirectionCS0,
				wheelAxleCS,
				gSuspensionRestLength,
				gWheelRadius,
				isFrontWheel, gWheelFriction, gRollInfluence);

			// Wheel 2
			connectionPointCS0 = Ogre::Vector3(
				-CUBE_HALF_EXTENTS+(0.3*gWheelWidth),
				connectionHeight,
				2*CUBE_HALF_EXTENTS-gWheelRadius);
			mVehicle->addWheel(
				wheelNode1,
				connectionPointCS0,
				wheelDirectionCS0,
				wheelAxleCS,
				gSuspensionRestLength,
				gWheelRadius,
				isFrontWheel, gWheelFriction, gRollInfluence);
					
			isFrontWheel = false;

			// Wheel 3
			connectionPointCS0 = Ogre::Vector3(
				-CUBE_HALF_EXTENTS+(0.3*gWheelWidth),
				connectionHeight,
				-2*CUBE_HALF_EXTENTS+gWheelRadius);
			mVehicle->addWheel(
				wheelNode2,
				connectionPointCS0,
				wheelDirectionCS0,
				wheelAxleCS,
				gSuspensionRestLength,
				gWheelRadius,
				isFrontWheel, gWheelFriction, gRollInfluence);

			// Wheel 4
			connectionPointCS0 = Ogre::Vector3(
				CUBE_HALF_EXTENTS-(0.3*gWheelWidth),
				connectionHeight,
				-2*CUBE_HALF_EXTENTS+gWheelRadius);
			mVehicle->addWheel(
				wheelNode3,
				connectionPointCS0,
				wheelDirectionCS0,
				wheelAxleCS,
				gSuspensionRestLength,
				gWheelRadius,
				isFrontWheel, gWheelFriction, gRollInfluence);
		}
	}
}
