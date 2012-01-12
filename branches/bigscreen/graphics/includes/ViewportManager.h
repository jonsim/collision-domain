/**
* @file		ViewportManager.h
* @brief	Manages the view ports for a window
*/
#ifndef VIEWPORTMANAGER_H
#define VIEWPORTMANAGER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include <math.h>

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class ViewportManager
{
public:
	ViewportManager	(int numViewports_P,Ogre::RenderWindow* window_P);
	Ogre::Viewport*	addViewport(Ogre::Camera* camera_P, bool main_P); //Set main = true for main camera
	//void			changeViewPortCamera(Ogre::Camera camera_P);
private:
	int					numViewports;
	int					zOrdering;
	int					sector;
	Ogre::RenderWindow*	window;
	float				viewportsPerSector;
	int					viewportsAdded;
};


#endif