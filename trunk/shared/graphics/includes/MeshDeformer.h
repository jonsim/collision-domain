#ifndef MESHDEFORMER_H
#define MESHDEFORMER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"

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

	void collisonDeform(Ogre::SceneNode *vehicle, const Ogre::Vector3 &epicentre, float damage, bool isFront);
	void traceNodeHierarchy(Ogre::SceneNode *rootnode);

    Ogre::ManualObject* drawLine(Ogre::SceneManager *sm, Ogre::SceneNode* parent, Ogre::Vector3 &start, Ogre::Vector3 &end, Ogre::ColourValue &col);
	~MeshDeformer(void);

protected :
	virtual void deformMesh(
		const Ogre::MeshPtr    mesh,
		const Ogre::Vector3    &epicentre,
		float                  damage,
        bool                   isFront,
        const Ogre::Vector3    &position   =  Ogre::Vector3::ZERO,
        const Ogre::Quaternion &orient     =  Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3    &scale      =  Ogre::Vector3::UNIT_SCALE
	);

	

	virtual unsigned time_seed();
};

#endif
