/**
 * @file	Car.cpp
 * @brief 	Contains the core methods and variables common to all different types of car.
                Extend or implement this class as required to create a functioning car.
                This class and its subclasses deal with the physics and the graphics which are
                car related and expose an interface to do stuff to the cars.
 */
#include "stdafx.h"
#include "stdafx.h"
#include "PhysicsCore.h"
#include "Car.h"
#include "GameCore.h"
#include "Gameplay.h"
#include "boost/algorithm/string.hpp"

#define WHEEL_FRICTION_CFM 0.6f

std::list<Ogre::Entity*>* Car::mClonedEntities = new std::list<Ogre::Entity*>;
std::list<Ogre::ResourceHandle>* Car::mMeshObjects = new std::list<Ogre::ResourceHandle>;

Car::Car (int uniqueID)
  : mGearSound(NULL),
    mBigScreenOverlayElement(NULL),
    mUniqueID(uniqueID)
{
    #ifdef COLLISION_DOMAIN_CLIENT
        mGearSound = GameCore::mAudioCore->getSoundInstance(GEAR_CHANGE, uniqueID, NULL);
        mCrashSound = GameCore::mAudioCore->getSoundInstance(CAR_CRASH, uniqueID, NULL);
    
        if ( GameCore::mClientGraphics->mBigScreen != NULL )
        {
            mBigScreenOverlayElement = GameCore::mClientGraphics->mBigScreen->createPlayerOverlayElement(uniqueID);
        }
    #endif
}

Car::~Car()
{
#ifdef COLLISION_DOMAIN_CLIENT
    GameCore::mAudioCore->deleteSoundInstance(mGearSound);
    GameCore::mAudioCore->deleteSoundInstance(mCrashSound);
    
    if ( GameCore::mClientGraphics->mBigScreen != NULL )
    {
        GameCore::mClientGraphics->mBigScreen->removeCarOverlayFromMap(mUniqueID);
    }
#endif
}

// Call with the location of the crash and the intensity between 0 and 1, ideally between 0 and 0.8
void Car::triggerCrashSoundAt(Ogre::Vector3 location, float intensity)
{
#ifdef COLLISION_DOMAIN_CLIENT
	intensity = intensity < 0 ? 0 : intensity;

	// maxVolume is limited in mAudioCore (0.8 currently)
	mCrashSound->setVolume(intensity);
	mCrashSound->setPosition(location);
	GameCore::mAudioCore->playSoundOrRestart(mCrashSound);
#endif
}

//#define DEBUG_SHOW_SKID

/// @brief  Takes the given CarSnapshot and positions this car as it specifies (velocity etc.).
/// @param  carSnapshot  The CarSnapshot specifying where and how to place the car.
void Car::restoreSnapshot(CarSnapshot *carSnapshot)
{
    moveTo(carSnapshot->mPosition, carSnapshot->mRotation);

    // After this the car will be moved and rotated as specified, but the current velocity
    // will be pointing in the wrong direction (if car is now rotated differently).
    mCarChassis->setAngularVelocity(carSnapshot->mAngularVelocity);
    mCarChassis->setLinearVelocity(carSnapshot->mLinearVelocity);

    mSteer = carSnapshot->mWheelPosition;
    applySteeringValue();
}


/// @brief  The CarSnapshot specifying the current location of this car which can be restored later if need be.
/// @return The CarSnapshot specifying where and how to place the car at its current location.
CarSnapshot *Car::getCarSnapshot()
{
    return new CarSnapshot(
        btVector3(mBodyNode->getPosition().x, mBodyNode->getPosition().y, mBodyNode->getPosition().z),
        mCarChassis->getOrientation(),
        mCarChassis->getAngularVelocity(),
        mCarChassis->getLinearVelocity(),
        mSteer);
}

float Car::getCarMph()
{
	return mVehicle->getCurrentSpeedKmHour() * 0.621371192;
}


/// @brief  Moves the car to the specified position keeping current rotation, velocity etc.
/// @param  position  The position to move to.
void Car::moveTo(const btVector3 &position)
{
    moveTo(position, mCarChassis->getOrientation());
    
    // now stop the car moving
    mCarChassis->setAngularVelocity(btVector3(0,0,0));
    mCarChassis->setLinearVelocity(btVector3(0,0,0));
    
    mSteer = 0;
    applySteeringValue();
}


/// @brief  This is PRIVATE for a reason. Without giving angular and linear velocity along with a new rotation
///         the rotation won't be applied nicely so the car will still keep moving in the original direction.
/// @param  position  The position to move to.
/// @param  rotation  The rotation to move to.
void Car::moveTo(const btVector3 &position, const btQuaternion &rotation)
{
    //GameCore::mPhysicsCore->getWorld()->removeRigidBody( mCarChassis );
    btTransform transform(rotation, position);
    //mCarChassis->setWorldTransform( transform );
    //mCarChassis->setInterpolationWorldTransform( transform );
    reset( mCarChassis, transform );
    /*if( mLeftDoorBody != NULL )
    {
        reset( mLeftDoorBody, transform );
        reset( mRightDoorBody, transform, false );
    }*/
    //GameCore::mPhysicsCore->getWorld()->addRigidBody( mCarChassis );
}


/// @brief  Called once every frame with new user input and updates steering from this.
/// @param  isLeft                  User input specifying if the left control is pressed.
/// @param  isRight                 User input specifying if the right control is pressed.
/// @param  secondsSinceLastFrame   For framerate independence as the wheel turning "accelerate" with keypresses.
void Car::steerInputTick(bool isLeft, bool isRight, Ogre::Real secondsSinceLastFrame)
{
    // process steering on both wheels (+1 = left, -1 = right)
    int leftRight = 0;
    if (isLeft)  leftRight += 1;
    if (isRight) leftRight -= 1;

    float calcIncrement = 0.0f;
    const float steeringScalingFactor = 1.0f / 60.0f;
    bool resetToZero = false;

    // we don't want to go straight to this steering value (i.e. apply acceleration to steer value)
    if (leftRight != 0)
    {
        // Apply steering increment normally
        // Else We are steering against the current wheel direction (i.e. back towards 0). Steer faster.
        if (mSteer * leftRight >= 0) calcIncrement = mSteerIncrement * leftRight;
        else calcIncrement = mSteerToZeroIncrement * leftRight;
    }
    else
    {
        // go back to zero
        if (mSteer >= 0)
        {
            if (mSteer >= mSteerToZeroIncrement * (secondsSinceLastFrame / steeringScalingFactor)) calcIncrement = -mSteerToZeroIncrement;
            else resetToZero = true;
        }
        else
        {
            if (mSteer <= -mSteerToZeroIncrement * (secondsSinceLastFrame / steeringScalingFactor)) calcIncrement = mSteerToZeroIncrement;
            else resetToZero = true;
        }
    }

    if (resetToZero) mSteer = 0;
    else
    {
        // Framerate independent wheel turning acceleration
        calcIncrement *= secondsSinceLastFrame / steeringScalingFactor;
        mSteer += calcIncrement;
    }

    applySteeringValue();
}


/// @brief  Sets the wheel position from the mSteer variable, and clamps mSteer to its bounds.
void Car::applySteeringValue()
{
    // don't steer too far! Use the clamps.
    float steer = mSteer > mSteerClamp ? mSteerClamp : (mSteer < -mSteerClamp ? -mSteerClamp : mSteer);

    mVehicle->setSteeringValue(steer, 0);
    mVehicle->setSteeringValue(steer, 1);

    mSteer = steer;
}


/// @brief  Called once every frame with new user input and updates forward/back engine forces from this.
/// @param  isForward  User input specifying if the forward control is pressed.
/// @param  isBack     User input specifying if the back control is pressed.
/// @param  isHand     User input specifying if the handbrake is on.
void Car::accelInputTick(bool isForward, bool isBack, bool isHand, Ogre::Real secondsSinceLastFrame)
{
    int forwardBack = 0;
    if (isForward) forwardBack += 1;
    if (isBack)    forwardBack -= 1;

    //float f = forwardBack < 0 ? mMaxBrakeForce : mMaxAccelForce;
    //mEngineForce = f * forwardBack;

    mEngineForce  = ( isForward ) ? mMaxAccelForce : 0;
    mBrakingForce = ( isBack    ) ? mMaxBrakeForce : 0;

    int doBrake = 0;

    if( isHand )
    {
        mVehicle->applyEngineForce( 0, 0 );
        mVehicle->applyEngineForce( 0, 1 );
        mVehicle->applyEngineForce( 0, 2 );
        mVehicle->applyEngineForce( 0, 3 );

        mVehicle->setBrake( 2* mMaxAccelForce, 2 );
        mVehicle->setBrake( 2* mMaxAccelForce, 3 );

    }
    else
    {
        // Reset brakes to 0
        for( int i = 0; i < 4; i ++ )
            mVehicle->setBrake( 0 , i );
    }

    // Loop through each wheel
    for( int i = 0; i < 4; i ++ )
    {
        // Skip wheels depending on car driving mode
        if( i < 2 && !mFrontWheelDrive ) continue;
        if( i > 1 && !mRearWheelDrive  ) continue;

        if( isHand ) continue;

        // This code is a bit of a mess to avoid really tight brake / reverse checks
        // on exact float values but it works!

        float fSpeed = this->mVehicle->getCurrentSpeedKmHour();
        if( fSpeed < 2 )                                                                // Brake / Reverse threshold between 0 and 1 kph
        {
            if( isBack )
            {
                mVehicle->applyEngineForce( mMaxAccelForce * -0.6, i );                 // Press brake - assume we want to reverse
                mCurrentGear = -1;
            }
            else
            {
                mVehicle->applyEngineForce( 0, i );                                     // Turn off assumed reverse
                if( mCurrentGear == -1 )
                    mCurrentGear = 1;
            }

            if( isForward )
            {
                if( fSpeed >= -2 )
                {
                    mVehicle->applyEngineForce( mEngineForce, i );                      // Press accel & moving forwards - accelerate
                    doBrake = 1;
                }
                else
                {
                    mVehicle->applyEngineForce( 0, i );                                 // Press accell & moving backwards - turn off accel
                    doBrake = 2;
                }
            }
            else
            {
                mVehicle->setBrake( 0, i );                         // Moving backwards and not pressing accel - turn off brake
            }

        }
        else                                                                            // Speed above threshold - driving forwards
        {
            if( isBack )
                mVehicle->applyEngineForce( 0, i );                                     // Turn off accel if you're pressing brake temporarily
            else
                mVehicle->applyEngineForce( mEngineForce, i );                          // otherwise normal force (simulate accel & brake together)
            doBrake = 1;
        }
    }

    if( doBrake > 0 && !isHand )
    {
        for( int i = 0; i < 4; i ++ )
            mVehicle->setBrake( doBrake == 1 ? mBrakingForce : mMaxBrakeForce , i );
    }
    else if( !isHand )
    {
        // Reset brakes to 0
        for( int i = 0; i < 4; i ++ )
            mVehicle->setBrake( 0 , i );
    }
	
    updateRPM();
}

/*
mph = (rpm * cir) / (gear * final * 88)
where rpm = engine rpm
cir = tyre cicumference, in feet
gear = gear ratio of your car
final = final drive ratio of your car
88 = combines several conversion factors
*/
void Car::updateRPM()
{
    // Check if we're in neutral (revving at start of race)
    if( mCurrentGear == 0 )
    {
        // Linearly increase engine RPM
        mEngineRPM += mMaxAccelForce / mRevLimit;

        // Fluctuate around the rev limiter value
        if( mEngineRPM > mRevLimit )
            mEngineRPM -= 500;
    }
    else if( mCurrentGear == -1 )
    {
        // Calculate MPH -> RPM for current gear
        float fRPMCir = 
            getCarMph() * ( mReverseRatio * mFinalDriveRatio * 88 );

        // Previous calcs give RPM * WheelCircumference in feet, so get RPM
        mEngineRPM = fRPMCir / (Ogre::Math::PI * (2 * mWheelRadius*3.2808399));

        if( mEngineRPM < mRevTick  ) mEngineRPM = ( mRevTick  - 200 ) + ( rand() % 400 );
        if( mEngineRPM > mRevLimit ) mEngineRPM = ( mRevLimit - 200 ) + ( rand() % 400 );
    }
    else
    {
        // Calculate MPH -> RPM for current gear
        float fRPMCir = 
            getCarMph() * ( mGearRatio[mCurrentGear-1] * mFinalDriveRatio * 88 );

        // Calculate MPH -> RPM for current gear (for shift-down check)
        float fPrevGear = mCurrentGear <= 1 ? fRPMCir :
            getCarMph() * ( mGearRatio[mCurrentGear-2] * mFinalDriveRatio * 88 );

        // Previous calcs give RPM * WheelCircumference in feet, so get RPM
        mEngineRPM = fRPMCir / (Ogre::Math::PI * (2 * mWheelRadius*3.2808399));
        fPrevGear /= (Ogre::Math::PI * (2 * mWheelRadius*3.2808399));

        bool justShiftedUp = false;

        // Check for shift-up
        if( mEngineRPM > mRevLimit - 1500 )
        {
            mEngineRPM = ( mRevLimit - 200 ) + ( rand() % 400 );
            if( mCurrentGear < mGearCount )
            {
                mCurrentGear ++;
                justShiftedUp = true;
#ifdef COLLISION_DOMAIN_CLIENT
                GameCore::mAudioCore->playSoundOrRestart(mGearSound);
#endif
            }
        }
        
        // Check for shift-down
        if( fPrevGear < mRevLimit - 1700 && mCurrentGear > 1 && !justShiftedUp )
        {
            mCurrentGear --;
#ifdef COLLISION_DOMAIN_CLIENT
            GameCore::mAudioCore->playSoundOrRestart(mGearSound);
#endif
        }
    }

    // Fluctuate about the engin rest level if RPM is lower
    if( mEngineRPM < mRevTick )
        mEngineRPM = ( mRevTick - 200 ) + ( rand() % 400 );

#ifdef DEBUG_SHOW_REVS
    CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/fps" );
    char szFPS[64];
    sprintf( szFPS,   "RPM: %f    Gear: %i", mEngineRPM, mCurrentGear );
    fps->setText( szFPS );
#endif
}

void Car::updateParticleSystems(Ogre::Real secondsSinceLastFrame)
{
	static float oldRPM = 0;

	// Calculate the new exhaust emission rate (from engine RPM).
	float exhaustRate = 0;
	//if (isForward)
	//{
	float dRdT = (mEngineRPM - oldRPM) / (secondsSinceLastFrame);  // differential  d(RPM) / d(T)
	if (dRdT > 1300)
		exhaustRate = (mEngineRPM / mRevLimit) * 250;
	//}
	oldRPM = mEngineRPM;

    // Calculate the new dust emission rate (from wheel slip/skid).
    float dustRate[4] = {0, 0, 0, 0};

    fricConst->calcWheelSkid();
    if (fricConst->getWheelSkid() < 0.1f)
    {
        fricConst->calcSlipAngle();

        for (int i = 0; i < 4; i++)
        {
            btScalar slipAngle = fricConst->getSlipAngle(i);
            if (slipAngle > 1.2f)
                dustRate[i] = slipAngle * 13;
        }
    }
    // Set the new particle emission rates.
	for (int i = 0; i < mExhaustSystem->getNumEmitters(); i++)
		mExhaustSystem->getEmitter(i)->setEmissionRate(exhaustRate);
	for (int i = 0; i < 4; i++)
		mDustSystem->getEmitter(i)->setEmissionRate(dustRate[i]);
    
}


/// @brief  If a node isnt already attached, attaches a new one, otherwise returns the current one
/// @return The node onto which a camera can be attached to observe the car. The parent of this node is
///         guaranteed to be the arm node.
Ogre::SceneNode *Car::attachCamNode()
{
    if (mCamNode != NULL)
        return mCamNode;

    // else we need to make a new camera
    mCamArmNode = mBodyNode->createChildSceneNode("CamArmNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mCamNode  = mCamArmNode->createChildSceneNode("CamNode"    + boost::lexical_cast<std::string>(mUniqueCarID));

    return mCamNode;
}


void Car::attachCollisionTickCallback(Player* player)
{
    mCarChassis->setUserPointer(player);
}


/// @brief Applies the force vector to the car through the node supplied.
/// @param node     The node to apply the force through. Use the chassis node if a central force is desired.
/// @param force    The force vector to apply.
void Car::applyForce(Ogre::SceneNode* node, Ogre::Vector3 force)
{
    btVector3 btForce(force.x, force.y, force.z);
    btVector3 btPos(node->getPosition().x, node->getPosition().y, node->getPosition().z);
    mCarChassis->applyImpulse(btForce, btPos);
}


/// @brief  Loads the given car mesh and attaches it to the given node. The given entity name is used, but appended
///         with this car's unique ID so that (forbidden) name collisions don't occur.
///         clones the meshes which correspond to deformable items on the car.
/// @param  entityName     Name which the imported mesh will be given.
/// @param  meshName       Name of the mesh which is to be imported.
/// @param  toAttachTo     The SceneNode which the mesh should be imported and attached to.
/// @param  isDeformable   Whether or not the item is deformable (whether it is to be cloned or not)
void Car::createGeometry(const std::string &entityName, const std::string &meshName, Ogre::SceneNode *toAttachTo, bool isDeformable)
{
    Ogre::Entity* entity;
	Ogre::MeshPtr disposableMeshPointer;
	std::string nameUnique = entityName + boost::lexical_cast<std::string>(mUniqueCarID);
	if(!isDeformable) {
		try
		{
			entity = GameCore::mSceneMgr->getEntity(nameUnique);
		}
		catch (Ogre::ItemIdentityException)
		{
			entity = GameCore::mSceneMgr->createEntity(nameUnique, meshName);
		}
	} else {
		disposableMeshPointer = GameCore::mSceneMgr->getEntity(entityName)->getMesh()->clone(nameUnique + "mesh");
		entity = GameCore::mSceneMgr->createEntity(nameUnique, disposableMeshPointer->getName());

        mClonedEntities->push_back(entity);
        mMeshObjects->push_back(disposableMeshPointer->getHandle());
	}

    int GEOMETRY_QUERY_MASK = 1<<2;
    entity->setQueryFlags(GEOMETRY_QUERY_MASK); // lets raytracing hit this object (for physics)
#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
    entity->setNormaliseNormals(true);
#endif // only applicable before shoggoth (1.5.0)

    entity->setCastShadows(true);
    toAttachTo->attachObject(entity);

}

void Car::setMaterial(const std::string &materialName, Ogre::SceneNode *attachedTo)
{
    Ogre::Entity* ent = (Ogre::Entity*) attachedTo->getAttachedObject(0);
    ent->setMaterialName(materialName);
}

void Car::reset( btRigidBody *body, btTransform &trans, bool dotrans )
{
    //GameCore::mPhysicsCore->getWorld()->removeRigidBody( body );

    if( dotrans )
    {
        body->setWorldTransform( trans );
        body->setInterpolationWorldTransform( trans );
    }

    //GameCore::mPhysicsCore->getWorld()->addRigidBody( body );

    body->setLinearVelocity( btVector3( 0, 0, 0 ) );
    body->setAngularVelocity( btVector3( 0, 0, 0 ) );
    GameCore::mPhysicsCore->getWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs( body->getBroadphaseHandle(), GameCore::mPhysicsCore->getWorld()->getDispatcher() );
}

#ifdef _WIN32
void Car::removePiece( Ogre::SceneNode *node, btRigidBody *body, PHYS_SHAPE shape, btVector3& offset )
#else
void Car::removePiece( Ogre::SceneNode *node, btRigidBody *body, PHYS_SHAPE shape, btVector3 offset )
#endif
{
    if( node->getParentSceneNode() != mBodyNode )
        return;

    mBodyNode->removeChild( node->getName() );
    GameCore::mSceneMgr->getRootSceneNode()->addChild( node );
    node->setPosition( mBodyNode->getPosition() );

    btTransform ltrans( btQuaternion::getIdentity(), offset );
    btMotionState *lstate = new BtOgre::RigidBodyState( node, mCarChassis->getWorldTransform() * ltrans, ltrans.inverse() );
    btVector3 linertia;
    GameCore::mPhysicsCore->getCollisionShape( shape )->calculateLocalInertia( 30.f, linertia );

    body = new btRigidBody( 30.f, lstate, GameCore::mPhysicsCore->getCollisionShape( shape ), linertia );

    body->setActivationState( ISLAND_SLEEPING );
    GameCore::mPhysicsCore->addRigidBody( body, COL_CAR, COL_ARENA | COL_CAR );
    body->setDamping( 0.2f, 0.5f );
    //body->setDamping( 100.2f, 100.5f );
    body->setActivationState( ISLAND_SLEEPING );

    //body->setWorldTransform( mVehicle->getChassisWorldTransform() );
    //btTransform tr( btQuaternion::getIdentity(), btVector3( 0, 10, 0 ) );
    //body->setWorldTransform( tr );
}

/********************************************************
 *  Wheel Constraint Class
 *  - deals with individual friction levels per wheel
 ********************************************************/

WheelFrictionConstraint::WheelFrictionConstraint( btRaycastVehicle *v, btRigidBody *r )
    : btTypedConstraint( CONTACT_CONSTRAINT_TYPE, *v->getRigidBody() )
{
    mVehicle = v; mbtRigidBody = r;
}

void WheelFrictionConstraint::getInfo1( btTypedConstraint::btConstraintInfo1* info )
{
    // Add two constraint rows for each wheel on the ground
    
    info->m_numConstraintRows = 0;
    for (int i = 0; i < mVehicle->getNumWheels(); ++i)
    {
        btWheelInfo& wheel_info = mVehicle->getWheelInfo(i);
        //if(!wheel_info)
        //{
        //	std::cout << "No wheel info" << std::endl;
        //	exit(3);
        //}
        mVehicle->rayCast( wheel_info );
        info->m_numConstraintRows += 2 * ( wheel_info.m_raycastInfo.m_isInContact );
    }
}

void WheelFrictionConstraint::getInfo2( btTypedConstraint::btConstraintInfo2* info )
{
    int row = 0;

    // Setup sideways friction.

    for( int i = 0; i < mVehicle->getNumWheels(); ++i )
    {
        btWheelInfo& wheel_info = mVehicle->getWheelInfo(i);
        mVehicle->rayCast( wheel_info );

        // Only if the wheel is on the ground:
        if( wheel_info.m_raycastInfo.m_isInContact == false )
            continue;

        int row_index = row++ * info->rowskip;

        // Set axis to be the direction of motion:
        const btVector3& axis = wheel_info.m_raycastInfo.m_wheelAxleWS;
        info->m_J1linearAxis[row_index]   = axis[0];
        info->m_J1linearAxis[row_index+1] = axis[1];
        info->m_J1linearAxis[row_index+2] = axis[2];

        // Set angular axis.
        btVector3 rel_pos = wheel_info.m_raycastInfo.m_contactPointWS - mbtRigidBody->getCenterOfMassPosition();
        info->m_J1angularAxis[row_index]   = rel_pos.cross(axis)[0];
        info->m_J1angularAxis[row_index+1] = rel_pos.cross(axis)[1];
        info->m_J1angularAxis[row_index+2] = rel_pos.cross(axis)[2];

        // Set constraint error (target relative velocity = 0.0)
        info->m_constraintError[row_index] = 0.0f;

        info->cfm[row_index] = WHEEL_FRICTION_CFM; // Set constraint force mixing
        //info->erp = 0.8f;
        // Set maximum friction force according to Coulomb's law
        // Substitute Pacejka here
        //btScalar max_friction = wheel_info.m_frictionSlip;//wheel_info.m_wheelsSuspensionForce * wheel_info.m_frictionSlip / info->fps;
        btScalar max_friction = wheel_info.m_frictionSlip * ( wheel_info.m_wheelsSuspensionForce / wheel_info.m_maxSuspensionForce );
        // Set friction limits.
        info->m_lowerLimit[row_index] = -max_friction;
        info->m_upperLimit[row_index] = max_friction;

    }

    // Setup forward friction.
    for (int i = 0; i < mVehicle->getNumWheels(); ++i)
    {
        const btWheelInfo& wheel_info = mVehicle->getWheelInfo(i);

        // Only if the wheel is on the ground:
        if( wheel_info.m_raycastInfo.m_isInContact == false )
            continue;

        int row_index = row++ * info->rowskip;

        // Set axis to be the direction of motion:
        btVector3 axis = wheel_info.m_raycastInfo.m_wheelAxleWS.cross( wheel_info.m_raycastInfo.m_wheelDirectionWS );

        info->m_J1linearAxis[row_index]   = axis[0];
        info->m_J1linearAxis[row_index+1] = axis[1];
        info->m_J1linearAxis[row_index+2] = axis[2];

        // Set angular axis.
        btVector3 rel_pos = wheel_info.m_raycastInfo.m_contactPointWS - mbtRigidBody->getCenterOfMassPosition();

        info->m_J1angularAxis[row_index]   = rel_pos.cross(axis)[0];
        info->m_J1angularAxis[row_index+1] = rel_pos.cross(axis)[1];
        info->m_J1angularAxis[row_index+2] = rel_pos.cross(axis)[2];

        // FIXME: Calculate the speed of the contact point on the wheel spinning.

        // Estimate the wheel's angular velocity = m_deltaRotation
        btScalar wheel_velocity = wheel_info.m_deltaRotation * wheel_info.m_wheelsRadius;
        //btScalar wheel_velocity = wheel_info.m_rotation * wheel_info.m_wheelsRadius;

        // Set constraint error (target relative velocity = 0.0)
        info->m_constraintError[row_index] = wheel_velocity;

        // Set constraint force mixing
        info->cfm[row_index] = WHEEL_FRICTION_CFM; 

        // Set maximum friction force
        //btScalar max_friction = wheel_info.m_frictionSlip; //wheel_info.m_wheelsSuspensionForce * wheel_info.m_frictionSlip / info->fps;
        btScalar max_friction = wheel_info.m_frictionSlip * ( wheel_info.m_wheelsSuspensionForce / wheel_info.m_maxSuspensionForce );

        // Set friction limits.
        info->m_lowerLimit[row_index] = -max_friction;
        info->m_upperLimit[row_index] =  max_friction;
    }

#if defined(COLLISION_DOMAIN_CLIENT) && defined(DEBUG_SHOW_SKID)
    if( GameCore::mPlayerPool->getLocalPlayer()->getCar() != NULL )
    {
        if( mVehicle == GameCore::mPlayerPool->getLocalPlayer()->getCar()->getVehicle() )
        {
            CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/fps" );
            char szFPS[32];
	        sprintf(szFPS,   "fr: %.2f", mVehicle->getBulletVehicle()->getWheelInfo(3).m_skidInfo );
	        fps->setText(szFPS);
        }
    }
#endif
}

void WheelFrictionConstraint::setParam(int num, btScalar value, int axis) { return; }
///return the local value of parameter
btScalar WheelFrictionConstraint::getParam(int num, int axis) const { return 0.0; }


btScalar WheelFrictionConstraint::calcSlipAngle( int wheelNum )
{
    btScalar avg_slip = 0.00f;
    const btWheelInfo& wheel_info = 
        mVehicle->getWheelInfo( wheelNum );

    // Get velocity of wheel relative to the ground

    btVector3 relPos = wheel_info.m_raycastInfo.m_contactPointWS - 
        mbtRigidBody->getCenterOfMassPosition();

    btVector3 vel = mbtRigidBody->getVelocityInLocalPoint( relPos );

    // Get the x and z velocity
    btScalar velX = wheel_info.m_raycastInfo.m_wheelAxleWS.dot( vel );
    btScalar velZ = -wheel_info.m_raycastInfo.m_wheelDirectionWS.dot( vel );

    // Calculate slip angle
    m_wheel_slip[wheelNum] = 1.0f + btFabs( btAtan2( velX, btFabs( velZ ) ) );

    return m_wheel_slip[wheelNum];
}

btScalar WheelFrictionConstraint::calcSlipAngle()
{
    btScalar avg_slip = 0.00f;

    for( int i = 0; i < mVehicle->getNumWheels(); ++i )
        avg_slip += calcSlipAngle( i );

    m_avg_slip = avg_slip / mVehicle->getNumWheels();

    return m_avg_slip;
}

btScalar WheelFrictionConstraint::calcWheelSkid( int wheelNum )
{
    const btWheelInfo& wheel_info = 
        mVehicle->getWheelInfo( wheelNum );

    m_wheel_skid[wheelNum] = wheel_info.m_skidInfo;

    return m_wheel_skid[wheelNum];
}

btScalar WheelFrictionConstraint::calcWheelSkid()
{
    btScalar avg_skid = 0.00f;

    for( int i = 0; i < mVehicle->getNumWheels(); ++i )
        avg_skid += calcWheelSkid( i );

    m_avg_skid = avg_skid / mVehicle->getNumWheels();

    return m_avg_skid;
}

void Car::readTuning( char *szFile )
{
    std::ifstream ifs( szFile );
    if( !ifs )
        return;

    std::ostringstream oss;
    oss << ifs.rdbuf();

    if( !ifs && ifs.eof() )
        return;

    std::string strContent( oss.str() );

    std::vector<std::string> vecLines;
    boost::split( vecLines, strContent, boost::is_any_of( "\n" ) );

    for( uint32_t i = 0; i < vecLines.size(); i ++ )
    {
        std::vector<std::string> tokens;
        boost::split( tokens, vecLines.at(i), boost::is_any_of( "\t " ) );

        log( "line is %s and numtok is %i", vecLines.at(i).c_str(), tokens.size() );

        if( tokens.size() < 2 )
            continue;

        if( tokens.at(0) == "mSuspensionStiffness" )
            mSuspensionStiffness = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mSuspensionDamping" )
            mSuspensionDamping = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mSuspensionCompression" )
            mSuspensionCompression = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mMaxSuspensionForce" )
            mMaxSuspensionForce = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mRollInfluence" )
            mRollInfluence = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mSuspensionRestLength" )
            mSuspensionRestLength = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mMaxSuspensionTravelCm" )
            mMaxSuspensionTravelCm = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mFrictionSlip" )
            mFrictionSlip = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mChassisLinearDamping" )
            mChassisLinearDamping = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mChassisAngularDamping" )
            mChassisAngularDamping = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mChassisRestitution" )
            mChassisRestitution = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mChassisFriction" )
            mChassisFriction = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mChassisMass" )
            mChassisMass = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mWheelRadius" )
            mWheelRadius = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mWheelWidth" )
            mWheelWidth = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mWheelFriction" )
            mWheelFriction = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mConnectionHeight" )
            mConnectionHeight = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mSteerIncrement" )
            mSteerIncrement = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mSteerToZeroIncrement" )
            mSteerToZeroIncrement = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mSteerClamp" )
            mSteerClamp = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mMaxAccelForce" )
            mMaxAccelForce = boost::lexical_cast<float>( tokens.at(1) );
        if( tokens.at(0) == "mMaxBrakeForce" )
            mMaxBrakeForce = boost::lexical_cast<float>( tokens.at(1) );
    }
}

//get the car's current position
Ogre::Vector3 Car::GetPos()
{
	//return mPlayerNode->_getDerivedPosition() + mPlayerNode->_getDerivedOrientation() *
	//	mPlayerNode->_getDerivedScale() * mBodyNode->_getDerivedPosition();
	return mBodyNode->getPosition();
}

//get the car's current heading
Ogre::Quaternion Car::GetHeading()
{
	return mBodyNode->getOrientation();
}


/********************************************************
 *  Car State Class
 *  - move graphical car body and wheels with physics
 ********************************************************/
CarState::CarState( Ogre::SceneNode *node )
    : RigidBodyState( node )
{
    mVehicle = NULL;
    for( int i = 0; i < 4; i ++ )
        mWheelNode[i] = NULL;
}

void CarState::setWorldTransform(const btTransform &in)
{
    RigidBodyState::setWorldTransform( in );
    if( mVehicle )
    {
        for( int i = 0; i < mVehicle->getNumWheels(); i ++ )
        {
            if( !mWheelNode[i] ) continue;

            mVehicle->updateWheelTransform( i, true );
            const btTransform& wt = mVehicle->getWheelInfo( i ).m_worldTransform;

            mWheelNode[i]->setPosition( BtOgre::Convert::toOgre( wt.getOrigin() ) );
            mWheelNode[i]->setOrientation( BtOgre::Convert::toOgre( wt.getRotation() ) );
        }
    }
}

void CarState::setVehicle( btRaycastVehicle *v )
{
    mVehicle = v;
}

void CarState::setWheel( int wheelnum, Ogre::SceneNode *node, const Ogre::Vector3 &connectionPoint )
{
    mWheelNode[wheelnum] = node;
    node->setPosition( connectionPoint );
}

Ogre::OverlayElement* Car::getBigScreenOverlayElement()
{
    return mBigScreenOverlayElement;
}

void Car::reinitBigScreenOverlayElementIfNull()
{
    #ifdef COLLISION_DOMAIN_CLIENT
        if ( !mBigScreenOverlayElement
            && GameCore::mClientGraphics->mBigScreen )
        {
            mBigScreenOverlayElement = GameCore::mClientGraphics->mBigScreen->createPlayerOverlayElement(mUniqueID);
        }
    #endif
}
