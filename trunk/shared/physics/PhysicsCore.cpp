/**
 * @file	PhysicsCore.cpp
 * @brief 	Contains the physics world and methods relating to it (currently there are not many such methods)
 */
#include "stdafx.h"
#include "SharedIncludes.h"

//#define DEBUG_FRAMES


/// @brief  Constructor to create physics stuff
/// @param  sceneMgr  The Ogre SceneManager which nodes can be attached to.
PhysicsCore::PhysicsCore()
{
    // Gravity is not in the normal sense. Acceleration will look "normal" when the value is set to
    // the number of units used for a model of height 1m
    mBulletGravity = Ogre::Vector3(0, -9.81f, 0);

    mBulletAlignedBox = Ogre::AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000), Ogre::Vector3(10000,  10000,  10000));
    mNumEntitiesInstanced = 0; // how many shapes are created

    // Start Bullet
    mBroadphase         = new btAxisSweep3( btVector3( -10000, -10000, -10000 ), btVector3( 1000, 1000, 1000 ) );
    mCollisionConfig    = new btDefaultCollisionConfiguration();
    mDispatcher         = new btCollisionDispatcher( mCollisionConfig );
    mSolver             = new btSequentialImpulseConstraintSolver();

    mBulletWorld        = new btDiscreteDynamicsWorld( mDispatcher, mBroadphase, mSolver, mCollisionConfig );

    mBulletWorld->setGravity( btVector3( 0, -9.81f, 0 ) );

    // add Debug info display tool
#ifdef DEBUG_FRAMES

    dbgDraw = new BtOgre::DebugDrawer( 
        GameCore::mSceneMgr->getRootSceneNode(), mBulletWorld );

    mBulletWorld->setDebugDrawer( dbgDraw );

#endif

    // lets get the callback for collisions every substep
    mPlayerCollisions = new PlayerCollisions();
    //mBulletWorld->setInternalTickCallback( preTickCallback, 0, true );
    mBulletWorld->setInternalTickCallback( postTickCallback, 0, false );
}


/// @brief  Destructor to clean up
PhysicsCore::~PhysicsCore(void)
{
    // OgreBullet physic delete - RigidBodies
    std::deque<btRigidBody*>::iterator itBody = mBodies.begin();
    while (mBodies.end() != itBody)
    {   
        delete *itBody;
        ++itBody;
    }
    // OgreBullet physic delete - Shapes
    mBodies.clear();
    
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
}


/// @brief scale the given scenenode to the default scale
/// @param n The scenenode to scale.
void PhysicsCore::auto_scale_scenenode (Ogre::SceneNode* n)
{
    n->scale(MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT);
}


void PhysicsCore::stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real fixedTimestep)
{
    //mWorld->stepSimulation(elapsedTime, maxSubSteps, fixedTimestep);
    mBulletWorld->stepSimulation( elapsedTime, maxSubSteps, fixedTimestep );
    mBulletWorld->debugDrawWorld();

#ifdef DEBUG_FRAMES
    dbgDraw->setDebugMode( 1 );
    dbgDraw->step();
#endif

    mPlayerCollisions->frameEventEnd();
}


void PhysicsCore::preTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    //printf("The world just ticked by %f seconds\n", (float)timeStep);
    //OutputDebugString("Pre Tick\n");
}


/// timeStep is the number of seconds (float) which the world just ticked by (this substep only)
void PhysicsCore::postTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    //OutputDebugString("Post Tick\n");
    //defaultPlaneBody->getBulletObject()->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// This function is called *within* stepSimulation. Let's build a list of the cars which collided so
    // that once the stepSim finishes all its substeps (this multiple times) that list can be read off
    
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);

		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

        short groupA = obA->getBroadphaseHandle()->m_collisionFilterGroup;
        short groupB = obB->getBroadphaseHandle()->m_collisionFilterGroup;

        // group of the wheels is the chassis group (I think ...)
        // mask of the wheels is always 00000001 (COL_CAR)
        // Car to Car collision
        if (groupA & COL_CAR && groupB & COL_CAR)
        {
            Player* playerA = static_cast<Player*>(obA->getUserPointer());
            Player* playerB = static_cast<Player*>(obB->getUserPointer());
            if (playerA && playerB) GameCore::mPhysicsCore->mPlayerCollisions->addCollision(playerA, playerB, contactManifold);
        }
        // Car to Powerup collision
#ifdef COLLISION_DOMAIN_SERVER
        else if (groupA & COL_CAR && groupB & COL_POWERUP || groupA & COL_POWERUP && groupB & COL_CAR)
        {
            Player* player = static_cast<Player*>((groupA & COL_CAR ? obA : obB)->getUserPointer());
            Powerup* powerup = static_cast<Powerup*>((groupA & COL_POWERUP ? obA : obB)->getUserPointer());
            if (powerup && player)
            {
                powerup->playerCollision(player);
                // If it's a random powerup, leave the collision func to deal with the send once the replacement is decided
                if( GameCore::mPowerupPool->getPowerupType( powerup->getIndex() ) != POWERUP_RANDOM )
                    GameCore::mNetworkCore->sendPowerupCollect( powerup->getIndex(), player );
            }
        }
#endif
        // Car to Arena collision
        else if (groupA & COL_CAR && groupB & COL_ARENA || groupA & COL_ARENA && groupB & COL_CAR)
        {
            //Player* player = static_cast<Player*>((groupA & COL_CAR ? obA : obB)->getUserPointer());
            //player->collisionTickCallback(1);
        }
        else
        {
            continue;
        }
	}
}


/// @brief  mNumEntitiesInstances should only ever be modified through this method! (Potential crashes otherwise).
/// @return an int which has never before been used (in an entity name).
int PhysicsCore::getUniqueEntityID()
{
    mNumEntitiesInstanced++;
    return mNumEntitiesInstanced;
}


void PhysicsCore::attachCollisionMesh( Ogre::SceneNode *targetNode, Ogre::String collisionMeshName, float scaling )
{
    createCollisionShapes();

    Ogre::Entity* collisionEntity = GameCore::mSceneMgr->createEntity("CollisionMesh" + getUniqueEntityID(), collisionMeshName);

    Ogre::Matrix4 collisionScaling(scaling, 0,       0,       0,
                                   0,       scaling, 0,       0,
                                   0,       0,       scaling, 0,
                                   0,       0,       0,       1);
    
    BtOgre::StaticMeshToShapeConverter collisionShapeConverter(collisionEntity, collisionScaling);
    btCollisionShape *collisionShape = collisionShapeConverter.createTrimesh();
    
    short collisionGroup = COL_ARENA;
    short collisionMask  = COL_CAR;

    BtOgre::RigidBodyState *collisionBodyState = new BtOgre::RigidBodyState( targetNode );
    btRigidBody *collisionBody = new btRigidBody( 0.0f, collisionBodyState, collisionShape );
    
    mBulletWorld->addRigidBody( collisionBody, collisionGroup, collisionMask );

    // push the created objects to the deques
    mBodies.push_back(collisionBody);
}

void PhysicsCore::createCollisionShapes()
{
    SimpleCoupeCar::createCollisionShapes();
    SmallCar::createCollisionShapes();
    TruckCar::createCollisionShapes();
}

void PhysicsCore::addRigidBody( btRigidBody *body, short colGroup, short colMask )
{
    mBulletWorld->addRigidBody( body, colGroup, colMask );
    mBodies.push_back( body );
}

bool PhysicsCore::removeBody( btRigidBody *body )
{
    std::deque<btRigidBody*>::iterator it = find( mBodies.begin(), mBodies.end(), body );
    
    if ( it == mBodies.end() )
        return false;
    else
    {
        //mBulletWorld->removeRigidBody( body );
        mBodies.erase( it );
        return true;
    }
}