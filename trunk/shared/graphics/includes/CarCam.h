/**
 * @file	CarCam.h
 * @brief 	Contains the controls for implementing an attractive camera chaser
 */
#ifndef CARCAM_H
#define CARCAM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Car.h"

class CarCam //: public PhysicsEntity
{
public:
						CarCam(Car* car, Ogre::Camera* cam, Ogre::SceneNode* actualCamNode,Ogre::SceneNode* followedObject);
	void				updatePosition(int XRotation, int YRotation);
private:
	Ogre::Camera*		mCam;
	Ogre::SceneNode*	mFollowedObject;
	Ogre::SceneNode*	mApproxCamNode;
	Ogre::SceneNode*	mActualCamNode;
	Car*				mCar;
	
};

#endif // #ifndef PLAYER_H
