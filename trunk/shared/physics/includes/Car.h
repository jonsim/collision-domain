/**
 * @file	Car.h
 * @brief 	Contains the core methods and variables common to all different types of car.
                Extend or implement this class as required to create a functioning car.
                This class and its subclasses deal with the physics and the graphics which are
                car related and expose an interface to do stuff to the cars.
 */
#ifndef __Car_h_
#define __Car_h_

#include "stdafx.h"
#include "Gameplay.h"
#include "BulletDynamics/ConstraintSolver/btTypedConstraint.h"
#include "Vehicle.h"
#include "CarSnapshot.h"
#include "SharedIncludes.h"
#include "Team.h"
#include "OgreOggSound.h"

using namespace OgreOggSound;

class Player;
class WheelFrictionConstraint;
class CarState;

/// CarType represents the type of car used as the player object (i.e. the model).
enum CarType
{
    CAR_BANGER = 0,
    CAR_SMALL  = 1,
    CAR_TRUCK  = 2,

    CAR_COUNT
};

/// CarSkin represents the texture applied to the player object.
enum TeamID;


class Car
{

public:
    static std::list<Ogre::Entity*> *mClonedEntities;
    static std::list<Ogre::ResourceHandle> *mMeshObjects;

    Car(int uniqueID);
    virtual ~Car();

    int getUniqueID() { return this->mUniqueID; }

    // = 0 methods not implemented by Car yet!
    virtual void playCarHorn() = 0;
    virtual void updateAudioPitchFrameEvent() = 0;
    virtual void louderLocalSounds() = 0;
    virtual void startEngineSound() = 0;
    void triggerCrashSoundAt(Ogre::Vector3 location, float intensity);

    virtual void updateTeam (TeamID tid) = 0;
    virtual void updateArena (ArenaID aid) = 0;
    virtual void loadDestroyedModel (void) = 0;
    virtual void makeBitsFallOff() = 0;
    virtual void removeCarPart(unsigned int part) = 0;

    // Overrideable methods, but you can use the generic Car method with all cars
    virtual Ogre::SceneNode *attachCamNode();
    virtual void steerInputTick(bool isLeft,    bool isRight,             Ogre::Real secondsSinceLastFrame);
    virtual void accelInputTick(bool isForward, bool isBack, bool isHand, Ogre::Real secondsSinceLastFrame);
    virtual void moveTo(const btVector3 &position);
    virtual void moveTo(const btVector3 &position, const btQuaternion &rotation);
    virtual void restoreSnapshot(CarSnapshot *carSnapshot);
    virtual CarSnapshot *getCarSnapshot();
	float getCarMph();
    float getGear() { return mCurrentGear; }
    btRaycastVehicle *getVehicle() { return mVehicle; }
    void attachCollisionTickCallback(Player* player);
    void applyForce(Ogre::SceneNode* node, Ogre::Vector3 force);
    void resetMass();
    void setMass( float newmass );
    void resetEngineForce() { mMaxAccelForce = mMaxAccelForceBuf; }
    void setEngineForce( float newforce ) { mMaxAccelForce *= newforce; }
    
    void readTuning( char *szFile );
    float getRPM() { return mEngineRPM; }
    float getRevLimit() { return mRevLimit; }

	float getMaxSpeed() { return mMaxSpeed; }
    
	//Overlay Element stuff for the big screen view
	Ogre::OverlayElement* getBigScreenOverlayElement();
    void reinitBigScreenOverlayElementIfNull();

#ifdef _WIN32
    virtual void removePiece( Ogre::SceneNode *node, btRigidBody *body, PHYS_SHAPE shape, btVector3& offset );
#else
	virtual void removePiece( Ogre::SceneNode *node, btRigidBody *body, PHYS_SHAPE shape, btVector3 offset );
#endif
    virtual void updateParticleSystems(Ogre::Real secondsSinceLastFrame);

	Ogre::Vector3 GetPos();
	Ogre::Quaternion GetHeading();
    inline Ogre::Vector3 getLinearVelocity() {
        btVector3 vel = mCarChassis->getLinearVelocity();
        return Ogre::Vector3(vel.x(), vel.y(), vel.z());
    }
	Ogre::SceneNode *mBodyNode;
    Ogre::SceneNode *mPlayerNode;


protected:
    void updateRPM();
    void createGeometry(const std::string &entityName, const std::string &meshName, Ogre::SceneNode *toAttachTo, bool isDeformable);
    void setMaterial(const std::string &materialName, Ogre::SceneNode *attachedTo);
    
//#ifdef COLLISION_DOMAIN_CLIENT
    OgreOggISound *mGearSound;
    OgreOggISound *mCrashSound;
//#endif

    // Camera node (if not null its attached to bodyNode) (attachCamNode())
    Ogre::SceneNode *mCamArmNode;
    Ogre::SceneNode *mCamNode;

	// Wheel node
    Ogre::SceneNode *mWheelsNode;

	// Particle systems
	Ogre::ParticleSystem* mExhaustSystem;
	Ogre::ParticleSystem* mDustSystem;
    Ogre::ParticleSystem* mSmokeSystem;
    Ogre::ParticleSystem* mFireSystem;

    // Friction constraint
    WheelFrictionConstraint *fricConst;

    // Data for whole class
    int mUniqueCarID;

    // mTuning related values
    float mSteer;
    float mEngineForce;
    float mBrakingForce;

    // mTuning fixed properties
    float mSuspensionStiffness;
    float mSuspensionDamping;
    float mSuspensionCompression;
    float mMaxSuspensionForce;
    float mRollInfluence;
    float mSuspensionRestLength;
    float mMaxSuspensionTravelCm;
    float mFrictionSlip;
	float mChassisLinearDamping;
	float mChassisAngularDamping;
	float mChassisRestitution;
	float mChassisFriction;
	float mChassisMass;

    float mWheelRadius;
    float mWheelWidth;
    float mWheelFriction;
    float mConnectionHeight;
    
    float mSteerIncrement;
    float mSteerToZeroIncrement;
    float mSteerClamp;

    float mMaxAccelForceBuf;
    float mMaxAccelForce;
    float mMaxBrakeForce;

	bool  mFrontWheelDrive;
	bool  mRearWheelDrive;

    int   mGearCount;
    int   mCurrentGear;
    float mGearRatio[9];
    float mReverseRatio;
    float mFinalDriveRatio;

    float mEngineRPM;
    float mRevTick;
    float mRevLimit;

	float mMaxSpeed;

    // Car physics objects
    btRigidBody                         *mCarChassis;
    btRigidBody                         *mLeftDoorBody;
    btRigidBody                         *mRightDoorBody;

    Vehicle                             *mVehicle;
    CarState                            *mState;
    btRaycastVehicle::btVehicleTuning    mTuning;
    btDefaultVehicleRaycaster           *mVehicleRayCaster;

    Car *testCar; 

    btHingeConstraint *leftDoorHinge; 
    btHingeConstraint *rightDoorHinge;


private:
	Ogre::OverlayElement *mBigScreenOverlayElement;
    int mUniqueID;

    void applySteeringValue();
    void reset( btRigidBody *body, btTransform &trans, bool dotrans = true );

    inline float rpm2rads(float f){ return f * 0.1047197f; }
    inline float rads2rpm(float f){ return f * 9.5492966f; }
};

class WheelFrictionConstraint : public btTypedConstraint
{
public:
    WheelFrictionConstraint( Vehicle *v, btRigidBody *r );
    virtual void getInfo1( btTypedConstraint::btConstraintInfo1* info );
    virtual void getInfo2( btTypedConstraint::btConstraintInfo2* info );

	///override the default global value of a parameter (such as ERP or CFM), optionally provide the axis (0..5). 
	///If no axis is provided, it uses the default axis for this constraint.
    virtual	void setParam(int num, btScalar value, int axis = -1);
	///return the local value of parameter
    virtual	btScalar getParam(int num, int axis = -1) const;

    Vehicle    *mVehicle;
    btRigidBody         *mbtRigidBody;
    
    // Note to Jamie: I made the two calc functions public to implement my stuff, do whatever you like with them this is your domain.
    btScalar calcWheelSkid();
    btScalar calcSlipAngle();
    
    btScalar getSlipAngle() { return m_avg_slip; }    
    btScalar getWheelSkid() { return m_avg_skid; }
    btScalar getSlipAngle( int wheelNum ) { 
        return (wheelNum < 4 && wheelNum >= 0) ? m_wheel_slip[wheelNum] : -1.00f; }
    btScalar getWheelSkid( int wheelNum ) {
        return (wheelNum < 4 && wheelNum >= 0) ? m_wheel_skid[wheelNum] : -1.00f; }

private:
    btScalar calcSlipAngle( int wheelNum );
    
    btScalar calcWheelSkid( int wheelNum );

    btScalar m_wheel_slip[4];
    btScalar m_avg_slip;

    btScalar m_wheel_skid[4];
    btScalar m_avg_skid;
};

class CarState : public BtOgre::RigidBodyState 
{
public:

    CarState( Ogre::SceneNode *node );

    virtual void    setWorldTransform( const btTransform &in );
    void            setVehicle( btRaycastVehicle *v );
    void            setWheel( int wheelnum, Ogre::SceneNode *node, const Ogre::Vector3 &connectionPoint );

private:

    Ogre::SceneNode *mWheelNode[4];
    btRaycastVehicle *mVehicle;

};

#endif // #ifndef __Car_h_

