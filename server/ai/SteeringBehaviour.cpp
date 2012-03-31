

#include "SteeringBehaviour.h"
#include "GameIncludes.h"

using namespace Ogre;
using namespace std;

//constructor
SteeringBehaviour::SteeringBehaviour(Player* agent) : mAiPlayer(agent)
{
    m_iFlags   = 0;
    mWeightArrive            = 1.0;
    mWeightFlee              = 3.0;
    mWeightSeek              = 3.0;
    mWeightPursuit           = 1.0;
    mWeightEvade             = 0.01;
    mWeightWander            = 0.1;
    mWeightObstacleAvoidance = 10.0;
    mWeightWallAvoidance     = 10.0;
    mWeightSteering          = 200;
    mWanderDistance = 2.0;
    mWanderJitter   = 40.0;
    mWanderRadius   = 100.0;
    mTargetPlayer1 = NULL;
    mTargetPlayer2 = NULL;
    mWanderTarget = Vector3::ZERO;
}

Vector3 SteeringBehaviour::Calculate(void)
{
  mSteeringForce = Ogre::Vector3::ZERO;
  //switch(m_SummingMethod)
  //{
    //case weighted_average:
      //mSteeringForce = CalculateWeighted() * mWeightSteering;
      //break;
    //case prioritized:
      mSteeringForce = CalculatePrioritized();
    //  break;
    //case dithered:
      //mSteeringForce = CalculateDithered() * mWeightSteering;
      //break;
    //default:
      //mSteeringForce = Ogre::Vector3::ZERO;
  //}
  return mSteeringForce;
}

Vector3 SteeringBehaviour::CalculatePrioritized(void)
{
      Vector3 force = Vector3::ZERO;

      /*if(On(obstacle_avoidance))
      {
        force = ObstacleAvoidance(mAiPlayer->ObstacleFound()) * mWeightObstacleAvoidance;
        if(!AccumulateForce(mSteeringForce, force))
            return mSteeringForce;
      }*/

      /*if(On(wall_avoidance))
      {
        force += WallAvoidance(mAiPlayer->Feeler(), mAiPlayer->WallHitPosition(), mAiPlayer->WallNormal()) * mWeightWallAvoidance;
      }*/

      if(On(flee))
      {
          force += Flee(GetFleeTarget()->getCar()->GetPos()) * mWeightFlee;
      }
      
      if(On(seek))
      {
        force = Seek(GetSeekTarget()->getCar()->GetPos());
        return force;
      }
      /*
      if(On(pursuit))
      {
        assert(mTargetPlayer1 && "pursuit target not assigned");
        force = Pursuit(mTargetPlayer1->getCar()) * mWeightPursuit;
        if(!AccumulateForce(mSteeringForce, force))
            return mSteeringForce;
      }

      if(On(evade))
      {
        assert(mTargetPlayer1 && "evader target not assigned");
        force = Evade(mTargetPlayer1->getCar) * mWeightEvade;
        if(!AccumulateForce(mSteeringForce, force))
            return mSteeringForce;
      }*/

      if(On(wander))
      {
        force += Wander() * mWeightWander;
      }

      return force;
}

Vector3 SteeringBehaviour::Seek(Vector3 TargetPos)
{
    return TargetPos;
}

Vector3 SteeringBehaviour::Flee(Vector3 TargetPos)
{
    const double PanicDistanceSq = pow(7.0,2.0);
    
    if(mAiPlayer->getCar()->GetPos().distance(TargetPos) > PanicDistanceSq)
        return Vector3::ZERO;

    Vector3 ret = mAiPlayer->getCar()->GetPos() - TargetPos;
    
    return ret;
}

/*
Vector3 SteeringBehaviour::Pursuit(Car* evader)
{
    //check if pursuit and evade vehicles are facing each other
    Vector3 ToEvader = evader->GetPos() - mAiPlayer->GetPos();
    double RelativeHeading = mAiPlayer->GetHeading().dotProduct(evader->GetHeading());
    if( (ToEvader.dotProduct(mAiPlayer->GetHeading()) > 0) && (RelativeHeading < -0.95))
        return Seek(evader->GetPos());

    //predict position of evading vehicle and set as target
    double LookAheadTime = ToEvader.length() / (mAiPlayer->GetMaxSpeed() + evader->GetSpeed());

    return Seek(evader->GetPos() + evader->GetVelocity() * LookAheadTime);
}

Ogre::Vector3 SteeringBehaviour::Evade(Car* pursuer)
{
    Vector3 ToPursuer = pursuer->GetPos() - mAiPlayer->GetPos();
    double LookAheadTime = ToPursuer.length() / (mAiPlayer->GetMaxSpeed() + pursuer->GetSpeed());

    return Flee(pursuer->GetPos() + pursuer->GetVelocity() * LookAheadTime);
}*/

//creates wandering behaviour
Ogre::Vector3 SteeringBehaviour::Wander()
{
    double JitterThisTimeSlice = mWanderJitter;
    mWanderTarget = mAiPlayer->getCar()->GetPos();
    
    mWanderTarget += Vector3(RandomClamped() * JitterThisTimeSlice,
                                     RandomClamped() * JitterThisTimeSlice,
                                     RandomClamped() * JitterThisTimeSlice);

    //create unit vector of target
    mWanderTarget.normalise();
    mWanderTarget *= mWanderRadius;

    Vector3 target = mWanderTarget + Vector3(mWanderDistance, 0, mWanderDistance); //Move the target in front of the agent
    Vector3 ret = target - mAiPlayer->getCar()->GetPos();

    //prevent the vehicle from floating/sinking
    ret.y = mAiPlayer->getCar()->GetPos().y;
    cout << ret.x << " " << ret.z << endl;
    return ret;
}

/*Vector3 SteeringBehaviour::ObstacleAvoidance(const BaseGameEntity* obstacle)
{
    Ogre::Vector3 SteeringForce = Ogre::Vector3::ZERO;
    const double brakingWeight = 0.2;

    if(obstacle)
    {
        double multiplier = 2 + (mDBoxLength - obstacle->GetPos().x)/mDBoxLength;
        SteeringForce.x = (obstacle->BRadius() - obstacle->GetPos().x) * brakingWeight;
        SteeringForce.z = (obstacle->BRadius() - obstacle->GetPos().z) * multiplier;
    }

    return SteeringForce;
}*/


Vector3 SteeringBehaviour::WallAvoidance(const Vector3 feeler, const Vector3 wallHit, const Vector3 normal)
{
    Vector3 ret = Ogre::Vector3::ZERO;
    Vector3 OverShoot     = feeler - wallHit;

    ret = normal * OverShoot.length();

    return ret;
}

