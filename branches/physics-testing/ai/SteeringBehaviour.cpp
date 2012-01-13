#include "SteeringBehaviour.h"

#include <Vehicle.h>
#include <BaseGameEntity.h>

using namespace std;

SteeringBehaviour::SteeringBehaviour(Vehicle* agent)
{
    mVehicle = agent;
    m_iFlags   = 0;
    mWeightArrive            = 1.0;
    mWeightFlee              = 1.0;
    mWeightSeek              = 1.0;
    mWeightPursuit           = 1.0;
    mWeightEvade             = 0.01;
    mWeightWander            = 1.0;
    mWeightObstacleAvoidance = 10.0;
    mWeightWallAvoidance     = 10.0;
    mWeightSteering          = 200;
    mWanderDistance = WanderDist;
    mWanderJitter   = WanderJitterPerSec;
    mWanderRadius   = WanderRad;
    meceleration  = fast;
    mSummingMethod = prioritized;
    mTarget1 = NULL;
    mTarget2 = NULL;
    mWanderTarget = Ogre::Vector3::ZERO;
    mDBoxLength = 10.0;
}

SteeringBehaviour::~SteeringBehaviour()
{
}

bool SteeringBehaviour::AccumulateForce(Ogre::Vector3 &RunningTot, Ogre::Vector3 ForceToAdd)
{
    double MagnitudeSoFar     = RunningTot.length();
    double MagnitudeRemaining = mVehicle->GetMaxForce() - MagnitudeSoFar;
    double MagnitudeToAdd = ForceToAdd.length();

    if(MagnitudeRemaining <= 0.0)
    	return false;

    if(MagnitudeToAdd < MagnitudeRemaining)
    	RunningTot += ForceToAdd;
    else
    	RunningTot += (ForceToAdd.normalisedCopy() * MagnitudeRemaining);

    return true;
}

Ogre::Vector3 SteeringBehaviour::Calculate(void)
{
  mSteeringForce = Ogre::Vector3::ZERO;
  //switch(m_SummingMethod)
  //{
    //case weighted_average:
      //mSteeringForce = CalculateWeighted() * mWeightSteering;
      //break;
    //case prioritized:
      mSteeringForce = CalculatePrioritized() * mWeightSteering;
    //  break;
    //case dithered:
      //mSteeringForce = CalculateDithered() * mWeightSteering;
      //break;
    //default:
      //mSteeringForce = Ogre::Vector3::ZERO;
  //}
  return mSteeringForce;
}

Ogre::Vector3 SteeringBehaviour::CalculatePrioritized(void)
{
	  Ogre::Vector3 force;

	  if(On(obstacle_avoidance))
	  {
		force = ObstacleAvoidance(mVehicle->ObstacleFound()) * mWeightObstacleAvoidance;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  /*if(On(wall_avoidance))
	  {
		force = WallAvoidance(mVehicle->Feeler(), mVehicle->WallHitPosition(), mVehicle->WallNormal()) * mWeightWallAvoidance;
		if(!AccumulateForce(mSteeringForce, force)) return mSteeringForce;
	  }*/

	  if(On(flee))
	  {
		force = Flee(mVehicle->TargetFlee()) * mWeightFlee;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  if(On(seek))
	  {
		force = Seek(mVehicle->TargetSeek()) * mWeightSeek;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  if(On(arrive))
	  {
		force = Arrive(mVehicle->TargetSeek(), meceleration) * mWeightArrive;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  if(On(pursuit))
	  {
		assert(mTarget1 && "pursuit target not assigned");
		force = Pursuit(mTarget1) * mWeightPursuit;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  if(On(evade))
	  {
		assert(mTarget1 && "evader target not assigned");
		force = Evade(mTarget1) * mWeightEvade;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  if(On(wander))
	  {
		force = Wander() * mWeightWander;
		if(!AccumulateForce(mSteeringForce, force))
			return mSteeringForce;
	  }

	  return mSteeringForce;
}

Ogre::Vector3 SteeringBehaviour::CalculateWeighted(void)
{
	  Ogre::Vector3 force;

	  if(On(obstacle_avoidance))
		  mSteeringForce += ObstacleAvoidance(mVehicle->ObstacleFound()) * mWeightObstacleAvoidance;

	  //if(On(wall_avoidance))
		//  mSteeringForce += WallAvoidance(mVehicle->Feeler(), mVehicle->WallHitPosition(), mVehicle->WallNormal()) * mWeightWallAvoidance;

	  if(On(seek))
		  mSteeringForce += Seek(mVehicle->TargetSeek()) * mWeightSeek;

	  if(On(flee))
		  mSteeringForce += Flee(mVehicle->TargetFlee()) * mWeightFlee;

	  if(On(arrive))
		  mSteeringForce += Arrive(mVehicle->TargetSeek(), meceleration) * mWeightArrive;

      if(On(wander))
		  mSteeringForce += Wander() * mWeightWander;

	  if(On(pursuit))
	  {
		assert(mTarget1 && "pursuit target not assigned");
		mSteeringForce += Pursuit(mTarget1) * mWeightPursuit;
	  }
	  if(On(evade))
	  {
		assert(mTarget1 && "evade target not assigned");
		mSteeringForce += Evade(mTarget1) * mWeightEvade;
	  }

	  return Truncate(mSteeringForce, mVehicle->GetMaxForce());
}

Ogre::Vector3 SteeringBehaviour::CalculateDithered(void)
{
    Ogre::Vector3 force;
    return mSteeringForce;
}

Ogre::Vector3 SteeringBehaviour::Seek(Ogre::Vector3 TargetPos)
{
    Ogre::Vector3 DesiredVelocity = (TargetPos - mVehicle->Pos()).normalisedCopy() * mVehicle->GetMaxSpeed();
    return (DesiredVelocity - mVehicle->Velocity());
}

Ogre::Vector3 SteeringBehaviour::Flee(Ogre::Vector3 TargetPos)
{
	const double PanicDistanceSq = pow(8,2);
	if(mVehicle->Pos().distance(TargetPos) > PanicDistanceSq)
		return Ogre::Vector3::ZERO;

	Ogre::Vector3 DesiredVelocity = (mVehicle->Pos() - TargetPos).normalisedCopy() * mVehicle->GetMaxSpeed();
	return (DesiredVelocity - mVehicle->Velocity());
}

Ogre::Vector3 SteeringBehaviour::Arrive(Ogre::Vector3 TargetPos, Deceleration deceleration)
{
    Ogre::Vector3 ToTarget = TargetPos - mVehicle->Pos();
    double dist = ToTarget.length();
	if(dist > 0)
	{
		const double DecelerationTweaker = 0.3;
		double speed = dist / ((double)deceleration * DecelerationTweaker);
		speed = MinOf(speed, mVehicle->GetMaxSpeed());
		Ogre::Vector3 DesiredVelocity = ToTarget * speed / dist;
		return (DesiredVelocity - mVehicle->Velocity());
	}

	return Ogre::Vector3::ZERO;
}

Ogre::Vector3 SteeringBehaviour::Pursuit(const Vehicle* evader)
{
	//check if pursuit and evade vehicles are facing each other
	Ogre::Vector3 ToEvader = evader->Pos() - mVehicle->Pos();
	double RelativeHeading = mVehicle->GetHeading().dotProduct(evader->GetHeading());
	if( (ToEvader.dotProduct(mVehicle->GetHeading()) > 0) && (RelativeHeading < -0.95))
		return Seek(evader->Pos());

	//predict position of evading vehicle and set as target
	double LookAheadTime = ToEvader.length() / (mVehicle->GetMaxSpeed() + evader->GetSpeed());

	return Seek(evader->Pos() + evader->Velocity() * LookAheadTime);
}

Ogre::Vector3 SteeringBehaviour::Evade(const Vehicle *pursuer)
{
    Vector3 ToPursuer = pursuer->Pos() - mVehicle->Pos();
    double LookAheadTime = ToPursuer.length() / (mVehicle->GetMaxSpeed() + pursuer->GetSpeed());

    return Flee(pursuer->Pos() + pursuer->Velocity() * LookAheadTime);
}

//creates wandering behaviour
Ogre::Vector3 SteeringBehaviour::Wander(void)
{
	double JitterThisTimeSlice = mWanderJitter;

	mWanderTarget = mVehicle->Pos();
	mWanderTarget += Ogre::Vector3(RandomClamped() * JitterThisTimeSlice,
									 RandomClamped() * JitterThisTimeSlice,
									 RandomClamped() * JitterThisTimeSlice);

	//create unit vector of target
	mWanderTarget.normalise();
	mWanderTarget *= mWanderRadius;

	Ogre::Vector3 target = mWanderTarget + Ogre::Vector3(mWanderDistance, 0, mWanderDistance); //Move the target in front of the agent
	Ogre::Vector3 ret = target - mVehicle->Pos();

	//prevent the vehicle from floating/sinking
	ret.y = mVehicle->Pos().y;

	return ret;
}

Ogre::Vector3 SteeringBehaviour::ObstacleAvoidance(const BaseGameEntity* obstacle)
{
    Ogre::Vector3 SteeringForce = Ogre::Vector3::ZERO;
	const double brakingWeight = 0.2;

	if(obstacle)
	{
		double multiplier = 2 + (mDBoxLength - obstacle->Pos().x)/mDBoxLength;
		SteeringForce.x = (obstacle->BRadius() - obstacle->Pos().x) * brakingWeight;
		SteeringForce.z = (obstacle->BRadius() - obstacle->Pos().z) * multiplier;
	}

	return SteeringForce;
}

/*
Ogre::Vector3 SteeringBehaviour::WallAvoidance(const Ogre::Vector3 feeler, const Ogre::Vector3 wallHit, const Ogre::Vector3 normal)
{
  Ogre::Vector3 SteeringForce = Ogre::Vector3::ZERO;
  Ogre::Vector3 OverShoot     = feeler - wallHit;

  #ifdef DEBUG
  if(mVehicle->ID()==0 && OverShoot.length() > 0) std::cerr << "Overshoot=" << OverShoot.length() << std::endl;
  #endif

  SteeringForce = normal * OverShoot.length();

  return SteeringForce;
}*/
