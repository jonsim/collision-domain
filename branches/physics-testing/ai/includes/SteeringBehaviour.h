/**
 * @file	SteeringBehaviours
 * @brief 	implements the various behaviours for the ai vehicles
 */

#ifndef STEERINGBEHAVIOUR_H
#define STEERINGBEHAVIOUR_H

#include <vector>
#include <string>
#include <list>
#include <cassert>

#include <OgreVector3.h>
#include <OgreMath.h>

#include <utils.h>


class Vehicle;
class BaseGameEntity;

//Constants
const double WanderRad          = 500;  ///The radius of the constraining circle for the wander behavior
const double WanderDist         = 2.0;  ///Distance the wander circle is projected in front of the agent
const double WanderJitterPerSec = 40.0; ///The maximum amount of displacement along the circle each frame
const double WayPointSeekDist   = 20;   ///Used in path following

class SteeringBehaviour
{
 public:
	SteeringBehaviour(Vehicle* agent);
	virtual ~SteeringBehaviour();
	Ogre::Vector3 Calculate(void);
	void SetDBoxLength(double length){ mDBoxLength = length; }
	double DBoxLength(void)const{ return mDBoxLength;}
	//set target methods
	void SetTargetAgent1(Vehicle *Agent){mTarget1 = Agent;}
	void SetTargetAgent2(Vehicle *Agent){mTarget2 = Agent;}
	void SetTarget(const Ogre::Vector3 t){mTarget = t;}

	enum summing_method{ weighted_average, prioritized, dithered};
	void SetSummingMethod(summing_method sm){mSummingMethod = sm;}

	//functions to turn on/off or check if on behaviours
	void FleeOn(){m_iFlags |= flee;}
	void SeekOn(){m_iFlags |= seek;}
	void ArriveOn(){m_iFlags |= arrive;}
	void PursuitOn(){m_iFlags |= pursuit;}
	void EvadeOn(){m_iFlags |= evade;}
	void WanderOn(){m_iFlags |= wander;}
	void ObstacleOn(){m_iFlags |= obstacle_avoidance;}
	void WallAvoidanceOn(){m_iFlags |= wall_avoidance;}
	void FleeOff()     {if(On(flee))    m_iFlags ^= flee;}
	void SeekOff()     {if(On(seek))    m_iFlags ^= seek;}
	void ArriveOff()   {if(On(arrive))  m_iFlags ^= arrive;}
	void PursuitOff()  {if(On(pursuit)) m_iFlags ^= pursuit;}
	void EvadeOff()    {if(On(evade))   m_iFlags ^= evade;}
	void WanderOff()   {if(On(wander))  m_iFlags ^= wander;}
	void ObstacleOff() {if(On(wander))  m_iFlags ^= obstacle_avoidance;}
	void WallAvoidanceOff() {if(On(wander))  m_iFlags ^= wall_avoidance;}
	bool isFleeOn(){return On(flee);}
	bool isSeekOn(){return On(seek);}
	bool isArriveOn(){return On(arrive);}
	bool isPursuitOn(){return On(pursuit);}
	bool isEvadeOn(){return On(evade);}
	bool isWanderOn(){return On(wander);}
	bool isObstacleOn(){return On(obstacle_avoidance);}
	bool isWallAvoidanceOn(){return On(wall_avoidance);}

private:
    enum behaviour_type
    {
      none               = 0x00000,
      seek               = 0x00002,
      flee               = 0x00004,
      arrive             = 0x00008,
      wander             = 0x00010,
      cohesion           = 0x00020,
      separation         = 0x00040,
      allignment         = 0x00080,
      obstacle_avoidance = 0x00100,
      wall_avoidance     = 0x00200,
      follow_path        = 0x00400,
      pursuit            = 0x00800,
      evade              = 0x01000,
      interpose          = 0x02000,
      hide               = 0x04000,
      flock              = 0x08000,
      offset_pursuit     = 0x10000,
    };

    Vehicle* mVehicle; ///A pointer to the owner of this instance
    Ogre::Vector3 mSteeringForce; ///The steering total force created by the combined effect of all the selected behaviors
    Ogre::Vector3 mTarget; ///The current target

    Vehicle* mTarget1; ///Some Agents pointers, for example, for pursuit
    Vehicle* mTarget2;

    //For wandering behavior
    double mWanderRadius; ///The radius of the circle projected in front of the agent
    double mWanderDistance; ///The distance the circle is projected from the agent
    double mWanderJitter; ///Maximum amount of random displacement that can be added to the target each frame.
    Ogre::Vector3 mWanderTarget; ///The position the target is going to when wandering

    //multipliers
    double mWeightSeek;
    double mWeightFlee;
    double mWeightArrive;
    double mWeightPursuit;
    double mWeightEvade;
    double mWeightWander;
    double mWeightSteering;
    double mWeightObstacleAvoidance;
    double mWeightWallAvoidance;

    //dection box length, used for obstacle avoidance
    double mDBoxLength;

    //flags used to indicate whether or not a behvaviour is on
    int m_iFlags;
    enum Deceleration{ slow = 3, normal = 2, fast = 1};
    Deceleration meceleration;
    summing_method mSummingMethod;

    Ogre::Vector3 CalculatePrioritized(void);
    Ogre::Vector3 CalculateWeighted(void);
    Ogre::Vector3 CalculateDithered(void);

    bool AccumulateForce(Ogre::Vector3 &RuningTot, Ogre::Vector3 ForceToAdd);

    bool On(behaviour_type bt){return (m_iFlags & bt) == bt;} ///Test if a specific bit of m_iFlags is set
    Ogre::Vector3 Seek(Ogre::Vector3 TargetPos);
    Ogre::Vector3 Flee(Ogre::Vector3 TargetPos);
    Ogre::Vector3 Arrive(Ogre::Vector3 TargetPos, Deceleration deceleration);
    Ogre::Vector3 Pursuit(const Vehicle* evader);
    Ogre::Vector3 Evade(const Vehicle* pursuer);
    Ogre::Vector3 Wander(void);
    Ogre::Vector3 ObstacleAvoidance(const BaseGameEntity* obstacle);
    /*Ogre::Vector3 WallAvoidance(const Ogre::Vector3 feeler, const Ogre::Vector3 wallHit, const Ogre::Vector3 normal);*/
};

#endif // STEERINGBEHAVIOR_H
