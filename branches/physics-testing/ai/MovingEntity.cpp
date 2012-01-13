#include "MovingEntity.h"

MovingEntity::MovingEntity(Ogre::Vector3 position, double radius,
                 Ogre::Vector3 velocity, double max_speed,
                 Ogre::Vector3 heading, double mass,
                 Ogre::Vector3 scale, double turn_rate,
                 double max_force)
: BaseGameEntity(0,position, radius)
{
  m_vHeading     = heading;
  m_vVelocity    = velocity;
  m_dMass        = mass;
  m_vSide        = m_vHeading.perpendicular();
  m_dMaxSpeed    = max_speed;
  m_dMaxTurnRate = turn_rate;
  m_dMaxForce    = max_force;
}

MovingEntity::~MovingEntity()
{
}

/**
* @brief Set's the entity heading and side vector
* First it will check if the given heading is not a vector of zero length.
* If it's a valid vector, it will set's the entity heading and side vector
*/
void MovingEntity::SetHeading(const Ogre::Vector3 new_heading)
{
  assert( (new_heading.squaredLength() - 1.0) < 0.00001);
  m_vHeading = new_heading;
  m_vSide    = m_vHeading.perpendicular();
}


/**
  * @brief Rotate's the entity heading and side vector
  * Given a target position, this method rotates the entity's heading
  * and side vectors by an amount not greater than m_dMaxTurnRate until
  * it directly faces the target
  * @remarks Mat uses a rotational Matrix, since we are using Ogre, we try to stick
  * with Ogre only functions for 3D and stuff, so we check if it's already headed to
  * the target, if not we use the Ogre::Vector3::getRotationTo() function, put the result under
  * the parameter result and return false, otherwise we return true;
  * @return true when heading is facing in the desired direction
  */
bool MovingEntity::RotateHeadingToFacePosition(Ogre::Vector3 target, Ogre::Quaternion &result)
{
  //Same as Mat
  Ogre::Vector3 toTarget = (target - m_vPos).normalisedCopy();
  double ang = acos(m_vHeading.dotProduct(toTarget));
  if(ang < 0.00001) return true;
  //Now we use quaternions (~ by what Mat made)
  result = (ang*m_vHeading).getRotationTo(target);
  return false;
}
