/**
 * @file	PhysicsCore.cpp
 * @brief 	Contains the physics world and methods relating to it (currently there are not many such methods)
 */
#include "stdafx.h"
#include "SharedIncludes.h"
#include "PhysicsCore.h"
#include "TruckCar.h"
#include "SimpleCoupeCar.h"
#include "SmallCar.h"
#include "GameCore.h"

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
    clearWorld();
    
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
                powerup->playerCollision(player);
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


btRigidBody* PhysicsCore::createArenaBody( Ogre::SceneNode *targetNode, ArenaID aid )
{
    short collisionGroup = COL_ARENA;
    short collisionMask  = COL_CAR | COL_POWERUP;

    btRigidBody *collisionBody;

    BtOgre::RigidBodyState *collisionBodyState = new BtOgre::RigidBodyState( targetNode );
    collisionBody = new btRigidBody( 0.0f, collisionBodyState, mShapes[PHYS_SHAPE_COLOSSEUM + aid] );
    
    mBulletWorld->addRigidBody( collisionBody, collisionGroup, collisionMask );

    // push the created objects to the deques
    mBodies.push_back(collisionBody);
    return collisionBody;
}

void PhysicsCore::createCollisionShapes()
{
#ifdef COLLISION_DOMAIN_SERVER
    GameCore::mServerGraphics->createArenaCollisionShapes();
#else
    GameCore::mClientGraphics->createArenaCollisionShapes();
#endif
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
    
    if ( it == mBodies.end() || body == NULL )
        return false;
    else
    {
        if( body->getMotionState() )
            delete body->getMotionState();

        GameCore::mPhysicsCore->getWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs( body->getBroadphaseHandle(), getWorld()->getDispatcher() );
        mBulletWorld->removeCollisionObject( body );
        mBodies.erase( it );

        if( body )
        {
            //delete body;
            body = NULL;
        }

        return true;
    }

}

void PhysicsCore::clearWorld()
{
    for( int i = mBulletWorld->getNumCollisionObjects() - 1; i >= 0; i -- )
    {
        btCollisionObject *obj = mBulletWorld->getCollisionObjectArray()[i];
        btRigidBody *body = btRigidBody::upcast( obj );
        removeBody( body );
    }

    mBodies.clear();

    /*for( int j = 0; j < PHYS_SHAPE_COUNT; j ++ )
    {
        btCollisionShape *shape = mShapes[j];
        if( shape )
            delete shape;
    }
    */
#ifdef COLLISION_DOMAIN_CLIENT
    //GameCore::mSceneMgr->getRootSceneNode()->removeAndDestroyAllChildren();
#endif
}

bool PhysicsCore::singleObjectRaytest(const btVector3& rayFrom, const btVector3& rayTo, btVector3& worldNormal, btVector3& worldHitPoint)
{
        btScalar closestHitResults = 1.f;

        btCollisionWorld::ClosestRayResultCallback resultCallback(rayFrom,rayTo);

        bool hasHit = false;
        btConvexCast::CastResult rayResult;
        btSphereShape pointShape(0.0f);
        btTransform rayFromTrans;
        btTransform rayToTrans;

        rayFromTrans.setIdentity();
        rayFromTrans.setOrigin(rayFrom);
        rayToTrans.setIdentity();
        rayToTrans.setOrigin(rayTo);

    int numObjects = 1;

    btCollisionShape* shapePtr[1];
    shapePtr[0] = mShapes[PHYS_SHAPE_COLOSSEUM];


    btTransform transforms[1];
    transforms[0].setIdentity();
    transforms[0].setOrigin(btVector3(0,0,0));

        for (int s = 0; s < numObjects; s++)
        {
                //comment-out next line to get all hits, instead of just the closest hit
                //resultCallback.m_closestHitFraction = 1.f;

                //do some culling, ray versus aabb
                btVector3 aabbMin,aabbMax;

                shapePtr[s]->getAabb(transforms[s],aabbMin,aabbMax);

                btScalar hitLambda = 1.f;

                btVector3 hitNormal;

                btCollisionObject       tmpObj;

                tmpObj.setWorldTransform(transforms[s]);



                if (btRayAabb(rayFrom,rayTo,aabbMin,aabbMax,hitLambda,hitNormal))
                {
                        //reset previous result

                        btCollisionWorld::rayTestSingle(rayFromTrans,rayToTrans, &tmpObj, shapePtr[s], transforms[s], resultCallback);
                        if (resultCallback.hasHit())
                        {
                worldHitPoint = resultCallback.m_hitPointWorld;

                                //float fog = 1.f - 0.1f * rayResult.m_fraction;
                                resultCallback.m_hitNormalWorld.normalize();//.m_normal.normalize();
                                worldNormal = resultCallback.m_hitNormalWorld;
                                //worldNormal = transforms[s].getBasis() *rayResult.m_normal;
                                worldNormal.normalize();
                                hasHit = true;
                        }
                }
        }

        return hasHit;
}
