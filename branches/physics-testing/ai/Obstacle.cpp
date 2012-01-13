#include "Obstacle.h"

Obstacle::Obstacle(Ogre::Vector3 position,double aabbradii,  Ogre::String name, Ogre::String meshName, Ogre::SceneManager *scene)
: BaseGameEntity(0)
{
  mSceneMgr   = scene;
  m_vPos      = position;
  m_sName     = name;
  m_sMeshName = meshName;
  m_dBoundingRadius = aabbradii;
  m_vNormal   = Ogre::Vector3::ZERO;
  CreateObstacle();
}
Obstacle::Obstacle(Ogre::Vector3 position,double aabbradii,  Ogre::SceneManager *scene, Ogre::SceneNode *node)
: BaseGameEntity(0)
{
  mSceneMgr = scene;
  m_vPos    = position;
  m_vNormal   = Ogre::Vector3::ZERO;
  m_dBoundingRadius = aabbradii;
  AttachUserObject(node);
}

Obstacle::Obstacle(Ogre::Vector3 position,double aabbradii,  Ogre::SceneManager *scene, Ogre::SceneNode *node, Ogre::Vector3 normal)
: BaseGameEntity(0)
{
  mSceneMgr = scene;
  m_vPos    = position;
  m_vNormal = normal;
  m_dBoundingRadius = aabbradii;
  AttachUserObject(node);
}

Obstacle::~Obstacle()
{
}

void Obstacle::AttachUserObject(Ogre::SceneNode *node)
{
  node->getAttachedObject(0)->setUserAny(Ogre::Any(this));
}

void Obstacle::CreateObstacle(void)
{
  Ogre::SceneNode *nodePtr = NULL;
  Ogre::Entity    *entPtr  = NULL;

  entPtr  = mSceneMgr->createEntity(m_sName+"Entity",m_sMeshName);
  nodePtr = mSceneMgr->getRootSceneNode()->createChildSceneNode(m_sName + "Node", m_vPos);

  entPtr->setCastShadows(true);
  //entPtr->setQueryFlags(static_type);

  nodePtr->setScale(m_dBoundingRadius/2, m_dBoundingRadius/2, m_dBoundingRadius/2);

  entPtr->setUserAny(Ogre::Any(this));
  nodePtr->attachObject(entPtr);

  #ifdef DEBUG
  nodePtr->showBoundingBox(true);
  #endif
}
