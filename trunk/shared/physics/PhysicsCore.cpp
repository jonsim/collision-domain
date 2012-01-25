/**
 * @file	PhysicsCore.cpp
 * @brief 	Contains the physics world and methods relating to it (currently there are not many such methods)
 */
#include "stdafx.h"
#include "SharedIncludes.h"


/// @brief  Constructor to create physics stuff
/// @param  sceneMgr  The Ogre SceneManager which nodes can be attached to.
PhysicsCore::PhysicsCore(Ogre::SceneManager* sceneMgr)
{
    mSceneMgr = sceneMgr;

    // Gravity is not in the normal sense. Acceleration will look "normal" when the value is set to
    // the number of units used for a model of height 1m
    mBulletGravity = Ogre::Vector3(0,-9.81,0);

    mBulletAlignedBox = Ogre::AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000), Ogre::Vector3(10000,  10000,  10000));
    mNumEntitiesInstanced = 0; // how many shapes are created

    // Start Bullet
    mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, mBulletAlignedBox, mBulletGravity);

    // add Debug info display tool
    debugDrawer = new OgreBulletCollisions::DebugDrawer();
    debugDrawer->setDrawWireframe(false);   // we want to see the Bullet containers
    mWorld->setDebugDrawer(debugDrawer);
    mWorld->setShowDebugShapes(true);      // enable it if you want to see the Bullet containers
    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject(static_cast <Ogre::SimpleRenderable *> (debugDrawer));

    // lets get the callback for collisions every substep
    //mWorld->getBulletDynamicsWorld()->setInternalTickCallback(preTickCallback, 0, true);
    mWorld->getBulletDynamicsWorld()->setInternalTickCallback(postTickCallback, 0, false);
}


/// @brief  Destructor to clean up
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


void PhysicsCore::preTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    //printf("The world just ticked by %f seconds\n", (float)timeStep);
    //OutputDebugString("Pre Tick\n");
}


/// timeStep is the number of seconds (float) which the world just ticked by (this substep only)
void PhysicsCore::postTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    //OutputDebugString("Post Tick\n");

	// This function is called *within* stepSimulation. Let's build a list of the cars which collided so
    // that once the stepSim finishes all its substeps (this multiple times) that list can be read off

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
	    
        int flagsA = obA->getCollisionFlags();
        int flagsB = obB->getCollisionFlags();
        

        if ((flagsA & VEHICLE_QUERY_MASK) && (flagsB & VEHICLE_QUERY_MASK))
        {
            //OutputDebugString("Collision\n"); // :-) it works! (but gives an odd number of outputs
        }



		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}

    /*btManifoldArray manifoldArray;
    btBroadphasePairArray& pairArray = ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
    int numPairs = pairArray.size();

    for (int i=0; i<numPairs; i++)
    {
        manifoldArray.clear();

        const btBroadphasePair& pair = pairArray[i];
        
        //unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
        btBroadphasePair* collisionPair = dynamicsWorld->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
        if (!collisionPair)
            continue;

        if (collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

        for (int j=0;j<manifoldArray.size();j++)
        {
            btPersistentManifold* manifold = manifoldArray[j];
            btScalar directionSign = manifold->getBody0() == m_ghostObject ? btScalar(-1.0) : btScalar(1.0);
            for (int p=0; p<manifold->getNumContacts(); p++)
            {
                const btManifoldPoint&pt = manifold->getContactPoint(p);
                if (pt.getDistance()<0.f)
                {
	                const btVector3& ptA = pt.getPositionWorldOnA();
	                const btVector3& ptB = pt.getPositionWorldOnB();
	                const btVector3& normalOnB = pt.m_normalWorldOnB;
	                /// work here
                }
            }
        }
    }*/
}


/// @brief  mNumEntitiesInstances should only ever be modified through this method! (Potential crashes otherwise).
/// @return an int which has never before been used (in an entity name).
int PhysicsCore::getUniqueEntityID()
{
    mNumEntitiesInstanced++;
    return mNumEntitiesInstanced;
}


/// @brief  Create the floor plane at y = 0 and add it to the physics world.
void PhysicsCore::createFloorPlane()
{
    OgreBulletCollisions::CollisionShape *Shape;
    Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 0); // (normal vector, distance)
    OgreBulletDynamics::RigidBody *defaultPlaneBody = new OgreBulletDynamics::RigidBody(
            "BasePlane",
            mWorld);

    defaultPlaneBody->setStaticShape(Shape, 0.1, 0.8); // (shape, restitution, friction)

    defaultPlaneBody->getBulletObject()->setCollisionFlags(STATIC_GEOMETRY_QUERY_MASK);

    // push the created objects to the deques
    mShapes.push_back(Shape);
    mBodies.push_back(defaultPlaneBody);
}


/// @brief  Create the walls at +2500 and -2500 and add them to the physics world.
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


/// @brief  Creates a cube and adds it to the physics world
/// @param  instanceName  Name to be used for the imported cube mesh.
/// @param  pos  Position for the resulting cube.
/// @param  q  Rotation for the resulting cube.
/// @param  size  Size for the resulting cube.
/// @param  bodyRestitution  How bouncy the cube is.
/// @param  bodyFriction  How slidey the cube is.
/// @param  bodyMass  How heavy the cube is.
void PhysicsCore::addCube(
        Ogre::String instanceName,
        Ogre::Vector3 pos,
        Ogre::Quaternion q,
        Ogre::Vector3 size,
        Ogre::Real bodyRestitution,
        Ogre::Real bodyFriction,
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


/// @brief  Creates a cube with velocity
void PhysicsCore::newBox(
        Ogre::SceneNode *node,
        Ogre::Vector3 position,
        Ogre::Vector3 size,
        Ogre::Vector3 cameraDirectionNormalised,
        float mass)
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

