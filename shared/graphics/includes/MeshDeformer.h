#ifndef MESHDEFORMER_H
#define MESHDEFORMER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"

/*#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreManualObject.h>*/

//#define DBOUT( s ) { std::ostringstream os_; os_ << s; OutputDebugString( os_.str().c_str() );  }

class MeshDeformer {
public:
    MeshDeformer(void);

    void collisonDeform(Ogre::SceneNode *vehicle, const Ogre::Vector3 &epicentre);
    void MeshDeformer::traceNodeHierarchy(Ogre::SceneNode *rootnode);
    ~MeshDeformer(void);

protected :
    virtual void deformMesh(
        const Ogre::MeshPtr    mesh,
        const Ogre::Vector3    &epicentre,
        const Ogre::Vector3    &position   =  Ogre::Vector3::ZERO,
        const Ogre::Quaternion &orient     =  Ogre::Quaternion::IDENTITY,
        const Ogre::Vector3    &scale      =  Ogre::Vector3::UNIT_SCALE
    );

    virtual Ogre::ManualObject* drawLine(Ogre::SceneManager* mSceneMgr, Ogre::Vector3 &start, Ogre::Vector3 &end);

    virtual unsigned time_seed();
};

#endif