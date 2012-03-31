/**
 * @file    CarCam.cpp
 * @brief     Contains the camera and updates to connect it to the car
 */

#include "stdafx.h"
#include "GameIncludes.h"
#include "CarCam.h"

#define SHIT_CAMERA 0

CarCam::CarCam(Car* car, Ogre::Camera* cam, Ogre::SceneNode* actualCamNode, Ogre::SceneNode* followedObject)
{
    mCam = cam;
    mFollowedObject = followedObject;
    mCar = car;
    mActualCamNode = actualCamNode;

    //Create a new node which we will attch our camera to
    //This node will attempt to follow the cam node 
    mApproxCamNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("ApproxCamFollower");
    //Some initial setup
    mApproxCamNode->setAutoTracking(true,mFollowedObject);
    mApproxCamNode->setFixedYawAxis(true);

    //Move the cam and the approx node to the same point
    //mCam->move(Ogre::Vector3(0,0,0));
    //mApproxCamNode->setPosition(Ogre::Vector3(0,0,0));
    
    //Attach the camera to the node
    //mApproxCamNode->translate(Ogre::Vector3(0,0,65));
    if (SHIT_CAMERA) mApproxCamNode->attachObject(mCam);
    else car->attachCamNode()->attachObject(mCam);
    //mCam->setFocalLength(4);
    
}

void CarCam::updatePosition(int XRotation, int YRotation)
{
    if (SHIT_CAMERA)
    {
    
        Ogre::Vector3 pos =  mFollowedObject->getPosition();
        Ogre::Quaternion orientation =  mFollowedObject->getOrientation();
    
        Ogre::Vector3 toMove = (pos-mApproxCamNode->getPosition());
        mApproxCamNode->translate(toMove);
        //mApproxCamNode->translate(Ogre::Vector3(0,0.5,0))
        //mApproxCamNode->translate(0.0,0.05,0);
        //+Ogre::Vector3(0,0.5,-62));
        //mApproxCamNode->setPosition(mActualCamNode->getPosition());
    }
    else
    {

    }
}