#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <BaseGameEntity.h>

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreVector3.h>
#include <OgreString.h>
#include <OgreAxisAlignedBox.h>

//#include <common.h>

class Obstacle : public BaseGameEntity
{
  public:
    Obstacle(Ogre::Vector3 position, double aabbradii, Ogre::String name, Ogre::String meshName, Ogre::SceneManager *scene);
    Obstacle(Ogre::Vector3 position, double aabbradii, Ogre::SceneManager *scene, Ogre::SceneNode *node, Ogre::Vector3 normal);
    Obstacle(Ogre::Vector3 position, double aabbradii, Ogre::SceneManager *scene, Ogre::SceneNode *node);
    virtual ~Obstacle();
    Ogre::Vector3 Normal(void)const{return m_vNormal;}
  private:
    void CreateObstacle(void);
    void AttachUserObject(Ogre::SceneNode *node);
  private:
    Ogre::SceneManager* mSceneMgr;
    Ogre::String m_sName;
    Ogre::String m_sMeshName;
    Ogre::Vector3 m_vNormal;
};

#endif // OBSTACLE_H
