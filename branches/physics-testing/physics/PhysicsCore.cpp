

#include "stdafx.h"
#include "PhysicsCore.h"


PhysicsCore::PhysicsCore(Ogre::SceneManager* sceneMgr)
{
    mSceneMgr = sceneMgr;

    // Gravity is not in the normal sense. Acceleration will look "normal" when the value is set to
    // the number of units used for a model of height 1m
    mBulletGravity = Ogre::Vector3(0,-82./*-9.81*/,0);



    mBulletAlignedBox = Ogre::AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000), Ogre::Vector3(10000,  10000,  10000));
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


PhysicsCore::~PhysicsCore(void)
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


/** @brief mNumEntitiesInstances should only ever be modified through this method! (Potential crashes otherwise)
    @return an int which has never before been used (in an entity name) */
int PhysicsCore::getUniqueEntityID()
{
    mNumEntitiesInstanced++;
    return mNumEntitiesInstanced;
}


void PhysicsCore::createFloorPlane()
{
    OgreBulletCollisions::CollisionShape *Shape;
    Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 12); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody = new OgreBulletDynamics::RigidBody(
            "BasePlane",
            mWorld);
    defaultPlaneBody->setStaticShape(Shape, 0.1, 0.8); // (shape, restitution, friction)
    // push the created objects to the deques
    mShapes.push_back(Shape);
    mBodies.push_back(defaultPlaneBody);
}


void PhysicsCore::createWallPlanes()
{
    // -2500 is good 2500 is bad. positive distances DO NOT WORK. Seriously, don't even bother
    OgreBulletCollisions::CollisionShape *Shape2;
    Shape2 = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,0,1), -2500); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody2 = new OgreBulletDynamics::RigidBody(
            "BasePlane2",
            mWorld);
    defaultPlaneBody2->setStaticShape(Shape2, 0.1, 0.8); // (shape, restitution, friction)
    //defaultPlaneBody2-> ->setPosition();
    // push the created objects to the deques
    mShapes.push_back(Shape2);
    mBodies.push_back(defaultPlaneBody2);


    OgreBulletCollisions::CollisionShape *Shape3;
    Shape3 = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,0,-1), -2500); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody3 = new OgreBulletDynamics::RigidBody(
            "BasePlane3",
            mWorld);
    defaultPlaneBody3->setStaticShape(Shape3, 0.1, 0.8); // (shape, restitution, friction)
    // push the created objects to the deques
    mShapes.push_back(Shape3);
    mBodies.push_back(defaultPlaneBody3);

    
    OgreBulletCollisions::CollisionShape *Shape4;
    Shape4 = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(1,0,0), -2500); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody4 = new OgreBulletDynamics::RigidBody(
            "BasePlane4",
            mWorld);
    defaultPlaneBody4->setStaticShape(Shape4, 0.1, 0.8); // (shape, restitution, friction)
    // push the created objects to the deques
    mShapes.push_back(Shape4);
    mBodies.push_back(defaultPlaneBody4);


    OgreBulletCollisions::CollisionShape *Shape5;
    Shape5 = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(-1,0,0), -2500); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody5 = new OgreBulletDynamics::RigidBody(
            "BasePlane5",
            mWorld);
    defaultPlaneBody5->setStaticShape(Shape5, 0.1, 0.8); // (shape, restitution, friction)
    // push the created objects to the deques
    mShapes.push_back(Shape5);
    mBodies.push_back(defaultPlaneBody5);
	
	
    //make the four walls collidable
    /*mPhysicsCore->addCube("wallObstacle1", Ogre::Vector3(0,  100, 2510), Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90)),
                          Ogre::Vector3::UNIT_X), Ogre::Vector3(2500, 10, 100), 0.3, 0.8, 0);
    mPhysicsCore->addCube("wallObstacle2", Ogre::Vector3(2510, 100, 10), Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90)),
                              Ogre::Vector3::UNIT_X), Ogre::Vector3(10, 2510, 100), 0.3, 0.8, 0);
    mPhysicsCore->addCube("wallObstacle3", Ogre::Vector3(0, 100, -2510), Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90)),
                                  Ogre::Vector3::UNIT_X), Ogre::Vector3(2510, 10, 100), 0.3, 0.8, 0);
    mPhysicsCore->addCube("wallObstacle4", Ogre::Vector3(-2510, 100, 0), Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90)),
                                  Ogre::Vector3::UNIT_X), Ogre::Vector3(10, 2500, 100), 0.3, 0.8, 0);*/
}


void PhysicsCore::addCube(Ogre::String instanceName,
                                       Ogre::Vector3 pos, Ogre::Quaternion q, Ogre::Vector3 size,
                                       Ogre::Real bodyRestitution, Ogre::Real bodyFriction,
                                       Ogre::Real bodyMass)
{
    Ogre::Entity *entity = mSceneMgr->createEntity(instanceName , "Bulletbox.mesh");
    // "Crate.mesh");
    // "Crate1.mesh");
    // "Crate2.mesh");


    entity->setQueryFlags (GEOMETRY_QUERY_MASK);
#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0)) // only applicable before shoggoth (1.5.0)
    entity->setNormaliseNormals(true);
#endif
    entity->setCastShadows(true);

    entity->setMaterialName("Bullet/box");

    OgreBulletCollisions::BoxCollisionShape *sceneCubeShape = new OgreBulletCollisions::BoxCollisionShape(size);

    OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(instanceName, mWorld);

    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode ()->createChildSceneNode ();
    node->attachObject (entity);

    defaultBody->setShape (node,  sceneCubeShape, bodyRestitution, bodyFriction, bodyMass, pos, q);

    mShapes.push_back(sceneCubeShape);
    mBodies.push_back(defaultBody);

}

void PhysicsCore::newBox(Ogre::SceneNode *node, Ogre::Vector3 position, Ogre::Vector3 size, Ogre::Vector3 cameraDirectionNormalised, float mass)
{
    size *= 0.05f; // don't forget to scale down the Bullet-box too
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
                          mass,          // dynamic bodymass
                          position,      // starting position of the box
                          Ogre::Quaternion(0,0,2,1));// orientation of the box
    defaultBody->setLinearVelocity(cameraDirectionNormalised * 7.0f ); // shooting speed
        // push the created objects to the deques
    mShapes.push_back(sceneBoxShape);
    mBodies.push_back(defaultBody);
}


/** @brief There is still a lot of hardcoded stuff in here, which will vary depending on mesh size and initial rotation
*/
OgreBulletDynamics::RaycastVehicle *PhysicsCore::newCar(Ogre::Vector3 carPosition,
                         Ogre::Vector3 chassisShift,
                         Ogre::SceneNode *carNode,
                         Ogre::SceneNode *wheelNodeFrontLeft,
                         Ogre::SceneNode *wheelNodeFrontRight,
                         Ogre::SceneNode *wheelNodeRearLeft,
                         Ogre::SceneNode *wheelNodeRearRight)
{
    OgreBulletDynamics::WheeledRigidBody *mCarChassis;
    OgreBulletDynamics::VehicleTuning    *mTuning;
    OgreBulletDynamics::VehicleRayCaster *mVehicleRayCaster;
    OgreBulletDynamics::RaycastVehicle   *mVehicle;
            
    static float gSuspensionStiffness = 20.f;
    static float gSuspensionDamping = 2.3f;
    static float gSuspensionCompression = 4.4f;
    static float gRollInfluence = 2.f;//1.0f;
    static float gSuspensionRestLength = 15.;
    static float gMaxSuspensionTravelCm = 900.0;
    static float gFrictionSlip = 10.5;

    static float gWheelRadius = 50.f; // very high values put front wheels at back
    static float gWheelWidth = 20.f; // very high values bring wheels together
    static float gWheelFriction = 1000;//1e30f;//1000;//1e30f;
    float connectionHeight = 28.f; // shift the wheels upwards

    OgreBulletCollisions::BoxCollisionShape* chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(50.f,36.f,140.0f));//Ogre::Vector3(1.f,0.75f,2.1f));
    OgreBulletCollisions::CompoundCollisionShape* compound = new OgreBulletCollisions::CompoundCollisionShape();

    compound->addChildShape(chassisShape, chassisShift);
    

    mCarChassis = new OgreBulletDynamics::WheeledRigidBody("carChassis" + Ogre::StringConverter::toString(getUniqueEntityID()), mWorld); // name given here needs to be unique to have more than one in the scene

    /*mShapes.push_back(chassisShape);
    mShapes.push_back(compound);
    mBodies.push_back(mCarChassis);*/

    mCarChassis->setShape (carNode, compound, 0.6, 0.6, 800, carPosition, Ogre::Quaternion::IDENTITY);
    mCarChassis->setDamping(0.2, 0.2);

    mCarChassis->disableDeactivation ();
    mTuning = new OgreBulletDynamics::VehicleTuning(gSuspensionStiffness,
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
            #define CUBE_HALF_EXTENTS 50 // 1
            bool isFrontWheel = true;

            // Wheel 1 - Front Left
            Ogre::Vector3 connectionPointCS0 (
                    CUBE_HALF_EXTENTS-(0.3*gWheelWidth),
                    connectionHeight,
                    2*CUBE_HALF_EXTENTS-gWheelRadius);
            mVehicle->addWheel(
                    wheelNodeFrontLeft,
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);

            // Wheel 2 - Front Right
            connectionPointCS0 = Ogre::Vector3(
                    -CUBE_HALF_EXTENTS+(0.3*gWheelWidth),
                    connectionHeight,
                    2*CUBE_HALF_EXTENTS-gWheelRadius);
            mVehicle->addWheel(
                    wheelNodeFrontRight,
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);
                    
            isFrontWheel = false;

            // Wheel 3 - Rear Right
            connectionPointCS0 = Ogre::Vector3(
                    -CUBE_HALF_EXTENTS+(0.3*gWheelWidth),
                    connectionHeight,
                    -2*CUBE_HALF_EXTENTS+gWheelRadius);
            mVehicle->addWheel(
                    wheelNodeRearRight,
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);

            // Wheel 4 - Rear Left
            connectionPointCS0 = Ogre::Vector3(
                    CUBE_HALF_EXTENTS-(0.3*gWheelWidth),
                    connectionHeight,
                    -2*CUBE_HALF_EXTENTS+gWheelRadius);
            mVehicle->addWheel(
                    wheelNodeRearLeft,
                    connectionPointCS0,
                    wheelDirectionCS0,
                    wheelAxleCS,
                    gSuspensionRestLength,
                    gWheelRadius,
                    isFrontWheel, gWheelFriction, gRollInfluence);
        }

        return mVehicle;
    }
}

