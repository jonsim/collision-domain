#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <list>
#include <set>
#include <string>

#include <Ogre.h>
#include <OgreVector3.h>
#include <OgreSceneQuery.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreMovableObject.h>
#include <OgreStringConverter.h>
#include <OgreIteratorWrappers.h>
#include <OgreResourceManager.h>

#include <MovingEntity.h>
#include <Obstacle.h>
//#include <Wall.h>
//#include <VehicleDebug.h>
#include <SteeringBehaviour.h>
#include <CollisionDetection.h>
#include "utils.h"
#include "common.h"
//#include <DynamicLines.h>
#include "PhysicsCore.h"
#include "Car.h"
#include "cars/BulletBuggyCar.h"
#include "cars/SimpleCoupeCar.h"
#include "Smoother.h"

class SteeringBehaviour;
class VehicleDebug;

using namespace Ogre;

class Vehicle : public MovingEntity
{
public:
    Vehicle(Vector3 position, double rotation,
            Vector3 velocity, double mass,
            double max_force, double max_speed,
            double max_turn_rate, double scale,
            SceneManager *scene, PhysicsCore *physicsCore
            , int id);
    virtual ~Vehicle();

    void Update(double time_elapsed);

    //Accessors
    SteeringBehaviour* const Steering(void)const{return m_pSteering;}

    Vector3    SmoothedHeading()const{return m_vSmoothedHeading;}
    bool        isSmoothingOn()const{return m_bSmoothingOn;}
    void        SmoothingOn(){m_bSmoothingOn = true;}
    void        SmoothingOff(){m_bSmoothingOn = false;}
    void        ToggleSmoothing(){m_bSmoothingOn = !m_bSmoothingOn;}

    Vector3 TargetFlee(void){return m_vTargetFlee;}
    void    SetTargetFlee(Vector3 target){m_vTargetFlee = target;}

    Vector3 TargetSeek(void){return m_vTargetSeek;}
    void    SetTargetSeek(Vector3 target){m_vTargetSeek = target;}

    //double WallDetectionLength(void)const{return m_dWallDetectionFeelerLength;}
    //void SetWallDetectionLength(double length){ m_dWallDetectionFeelerLength = length;}

    inline double TimeElapsed(void)const{return m_dTimeElapsed;}

    BaseGameEntity* ObstacleFound(void)const{return m_pFoundObstacle;}

    Vector3 Feeler(void)const{ return m_vFeelerPos;}
    Vector3 WallHitPosition(void)const{return m_vWallHitPosition;}
    Vector3 WallNormal(void)const{return m_vWallNormal;}

    std::vector<Vector3> GetFeelersPosition(void)const{ return m_vFeelers;}
    //Wall* GetWall(void)const{ return m_pFoundWall;}*/

  private:
    Vehicle(const Vehicle&);
    Vehicle& operator=(const Vehicle&);

    /**
    * @brief Create feelers for wall avoidance
    * This function will create 3 feelers from the vehicle, that check for
    * wall avoidance and put them in a std::vector.
    */
    //void CreateFeelers(void);
    /**
    * @brief Finds a wall intersection point
    * Using the CollisionDecetion::RaycastFromPoint() function, it searches for
    * wall detection from the feeler position to the vehicle position, if
    * it finds, it returs the point of intersection, otherwise it returns Vector3::ZERO
    * @return The point of intersection, or Vector3::ZERO
    */
    //Vector3 FindWalls(void);

    //Finds the closest obstacle
    Vector3 FindCloseObstacle(void);

    //void DebugFeeler(bool found, SceneNode *flr);


  //Variables
  private:
    SceneManager *mSceneMgr;

    //VehicleDebug *m_pDebug; ///Holds a pointer to an instance of the debug class

    SteeringBehaviour* m_pSteering; ///Holds the SteeringBehavior class
    Car * mCar;
    Vector3 m_vTargetFlee; ///A pointer to the position to flee
    Vector3 m_vTargetSeek; ///A pointer to the position to seek into or arrive into


    double m_dTimeElapsed; ///Keeps a track of the most recent update time.

    CollisionDetection* m_pCollision; ///Handles collision detection

    std::vector<Vector3> m_vFeelers; ///A container for all the feelers
    double m_dWallDetectionFeelerLength; ///How big the feeler is
    Vector3 m_vWallHitPosition; ///Were did the feeler hitted
    Vector3 m_vFeelerPos; ///What's the position of the hitting feeler
    Vector3 m_vWallNormal; ///What's the normal of the wall the feeler hitted
    //Wall *m_pFoundWall; ///Debug wall

    double m_dDBoxLength; ///The Detection Box Length, used for obstacle detection
    Obstacle* m_pFoundObstacle; ///What's the obstacle found*/

    //average of the vehicle's heading vector smoothed over the last few frames
    Vector3 m_vSmoothedHeading;
    bool m_bSmoothingOn;
    Smoother<Vector3>*  m_pHeadingSmoother;
};

#endif // VEHICLE_H
