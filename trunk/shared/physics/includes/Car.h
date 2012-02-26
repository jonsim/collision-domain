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
#include "SharedIncludes.h"
#include "BulletDynamics/ConstraintSolver/btTypedConstraint.h"

class Player;
class WheelFrictionConstraint;

class Car
{

#ifdef COLLISON_DOMAIN_CLIENT
    friend class SpawnScreen;
#endif

public:
    // = 0 methods not implemented by Car yet!
    virtual void playCarHorn() = 0;

    // Overrideable methods, but you can use the generic Car method with all cars
    virtual Ogre::SceneNode *attachCamNode();
    virtual void steerInputTick(bool isLeft, bool isRight, Ogre::Real secondsSinceLastFrame, float targetPhysicsFrameRate);
    virtual void accelInputTick(bool isForward, bool isBack, bool isHand, Ogre::Real secondsSinceLastFrame);
    virtual void moveTo(const btVector3 &position);
    virtual void restoreSnapshot(CarSnapshot *carSnapshot);
    virtual CarSnapshot *getCarSnapshot();
	float getCarMph();
    float getGear() { return mCurrentGear; }
    btRaycastVehicle *getVehicle() { return mVehicle; }
    void attachCollisionTickCallback(Player* player);
    
    void readTuning( char *szFile );
    float getRPM();

	Ogre::SceneNode *mBodyNode;
    Ogre::SceneNode *mPlayerNode;


protected:
    void createGeometry(const std::string &entityName, const std::string &meshName, Ogre::SceneNode *toAttachTo);
    
    OgreOggISound *mHornSound;

    // Camera node (if not null its attached to bodyNode) (attachCamNode())
    Ogre::SceneNode *mCamArmNode;
    Ogre::SceneNode *mCamNode;

	// Wheel node
    Ogre::SceneNode *mWheelsNode;

	// Particle systems
	Ogre::ParticleSystem* mExhaustSystem;
	Ogre::ParticleSystem* mDustSystem;

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


    // Car physics objects

    btBoxShape                          *chassisShape;
    btCompoundShape                     *compoundChassisShape;

    btRigidBody                         *mCarChassis;
    btRigidBody                         *mLeftDoorBody;
    btRigidBody                         *mRightDoorBody;

    btRaycastVehicle                    *mVehicle;
    btRaycastVehicle::btVehicleTuning    mTuning;
    btDefaultVehicleRaycaster           *mVehicleRayCaster;

    Car *testCar; 

    btHingeConstraint *leftDoorHinge; 
    btHingeConstraint *rightDoorHinge;


private:
    void applySteeringValue();
    void moveTo(const btVector3 &position, const btQuaternion &rotation);

    void updateRPM();

    void reset( btRigidBody *body, btTransform &trans, bool dotrans = true );
    

    inline float rpm2rads(float f){ return f * 0.1047197f; }
    inline float rads2rpm(float f){ return f * 9.5492966f; }
};

class WheelFrictionConstraint : public btTypedConstraint
{
public:
    WheelFrictionConstraint( btRaycastVehicle *v, btRigidBody *r );
    virtual void getInfo1( btTypedConstraint::btConstraintInfo1* info );
    virtual void getInfo2( btTypedConstraint::btConstraintInfo2* info );

	///override the default global value of a parameter (such as ERP or CFM), optionally provide the axis (0..5). 
	///If no axis is provided, it uses the default axis for this constraint.
    virtual	void setParam(int num, btScalar value, int axis = -1);
	///return the local value of parameter
    virtual	btScalar getParam(int num, int axis = -1) const;

    btRaycastVehicle    *mVehicle;
    btRigidBody         *mbtRigidBody;

    btScalar getSlipAngle( int wheelNum ) { 
        return (wheelNum < 4 && wheelNum >= 0) ? m_wheel_slip[wheelNum] : -1.00f; }

    btScalar getSlipAngle() { return m_avg_slip; }

    btScalar getWheelSkid() { return m_avg_skid; }

    btScalar getWheelSkid( int wheelNum ) {
        return (wheelNum < 4 && wheelNum >= 0) ? m_wheel_skid[wheelNum] : -1.00f; }

private:
    btScalar calcSlipAngle( int wheelNum );
    btScalar calcSlipAngle();
    
    btScalar calcWheelSkid( int wheelNum );
    btScalar calcWheelSkid();

    btScalar m_wheel_slip[4];
    btScalar m_avg_slip;

    btScalar m_wheel_skid[4];
    btScalar m_avg_skid;
};

#endif // #ifndef __Car_h_

