#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include <OgreVector3.h>
#include <OgreQuaternion.h>

#include "BaseGameEntity.h"
#include "utils.h"

class MovingEntity : public BaseGameEntity
{
protected:
    Ogre::Vector3 m_vVelocity;
    Ogre::Vector3 m_vHeading;
    //A vector perpendicular to the heading
    Ogre::Vector3 m_vSide;
    double m_dMass;
    double m_dMaxSpeed;
    double m_dMaxForce;
    double m_dMaxTurnRate;

public:
    MovingEntity(Ogre::Vector3 position, double radius,
                 Ogre::Vector3 velocity, double max_speed,
                 Ogre::Vector3 heading, double mass,
                 Ogre::Vector3 scale, double turn_rate,
                 double max_force);

    virtual ~MovingEntity();

    Ogre::Vector3 Velocity(void)const{return m_vVelocity;}
    void          SetVelocity(const Ogre::Vector3& v){m_vVelocity = v;}

    double GetMass(void) const{return m_dMass;}

    Ogre::Vector3 Side(void) const{return m_vSide;}

    double GetMaxSpeed(void) const{return m_dMaxSpeed;}
    void   SetMaxSpeed(double new_speed){m_dMaxSpeed = new_speed;}

    double GetMaxForce(void) const{return m_dMaxForce;}
    void   SetMaxForce(double mf){m_dMaxForce = mf;}

    bool   IsSpeedMaxedOut(void) const{return m_dMaxSpeed*m_dMaxSpeed >= m_vVelocity.squaredLength();}
    double GetSpeed(void) const{return m_vVelocity.length();}
    double SpeedSq(void) const{return m_vVelocity.squaredLength();}

    Ogre::Vector3 GetHeading(void) const{return m_vHeading;}
    void          SetHeading(const Ogre::Vector3 new_heading);
    bool          RotateHeadingToFacePosition(Ogre::Vector3 target, Ogre::Quaternion &result);

    double GetMaxTurnRate(void)const{return m_dMaxTurnRate;}
    void   SetMaxTurnRate(double val){m_dMaxTurnRate = val;}
};

#endif
