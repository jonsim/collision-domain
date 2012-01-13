#include "BaseGameEntity.h"

BaseGameEntity::BaseGameEntity()
{
  m_ID              = NextValidID();
  m_dBoundingRadius = 0.0;
  m_vPos            = Ogre::Vector3::ZERO;
  m_vScale          = Ogre::Vector3(1,1,1);
  m_EntityType      = default_entity_type;
  m_bTag            = false;
}

BaseGameEntity::BaseGameEntity(int entity_type)
{
  m_ID              = NextValidID();
  m_dBoundingRadius = 0.0;
  m_vPos            = Ogre::Vector3::ZERO;
  m_vScale          = Ogre::Vector3(1,1,1);
  m_EntityType      = entity_type;
  m_bTag            = false;
}

BaseGameEntity::BaseGameEntity(int entity_type, Ogre::Vector3 pos, double r)
{
  m_ID              = NextValidID();
  m_dBoundingRadius = r;
  m_vPos            = pos;
  m_vScale          = Ogre::Vector3(1,1,1);
  m_EntityType      = entity_type;
  m_bTag            = false;
}

BaseGameEntity::BaseGameEntity(int entity_type, int ForcedID)
{
  m_ID              = ForcedID;
  m_dBoundingRadius = 0.0;
  m_vPos            = Ogre::Vector3::ZERO;
  m_vScale          = Ogre::Vector3(1,1,1);
  m_EntityType      = entity_type;
  m_bTag            = false;
}

BaseGameEntity::~BaseGameEntity()
{
}

double MaxOf3(double x, double y, double z)
{
	if(x >= y && x >= z)
		return x;
	else if(y >= x && y >= z)
		return y;

	return z;
}

void BaseGameEntity::SetScale(Ogre::Vector3 val)
{
  m_dBoundingRadius *= MaxOf3(val.x, val.y, val.z)/MaxOf3(m_vScale.x, m_vScale.y, m_vScale.z);
  m_vScale = val;
}

void BaseGameEntity::SetScale(double val)
{
  m_dBoundingRadius *= (val/MaxOf3(m_vScale.x, m_vScale.y, m_vScale.z));
  m_vScale = Ogre::Vector3(val,val,val);
}
