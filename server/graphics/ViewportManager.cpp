/**
* @file		ViewportManager.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "ViewportManager.h"

/*-------------------- FUNCTION DEFINITIONS --------------------*/
ViewportManager::ViewportManager(int numViewports_P, Ogre::RenderWindow* window_P)
{
	numViewports = numViewports_P;
	zOrdering = 1;
	window = window_P;
	viewportsAdded = 0;
}

Ogre::Viewport* ViewportManager::addViewport(Ogre::Camera* camera_P, bool main_P)
{
	zOrdering++; //Keep track of Z-Ordering
	float widthOfViewport	= 0.2f;
	float heightOfViewport	= 0.2f;

	if(main_P) {
		float x = widthOfViewport;
		float y = 0.0;
		float widthOfViewportMain	= 1-(widthOfViewport*2);
		float heightOfViewportMain	= 1;

		//Ogre::Viewport* tmp = window->addViewport(camera_P,zOrdering,x,y,widthOfViewportMain,heightOfViewportMain);
		Ogre::Viewport* tmp = window->addViewport(camera_P);
		camera_P->setAspectRatio(Ogre::Real(tmp->getActualWidth()) / Ogre::Real(tmp->getActualHeight()));
		return tmp;
	} else {

		float x;

		if((viewportsAdded % 2) == 0) {
			x = 0;
		} else {
			x = 0.8f;
		}

		float y = 1.0f - ((floor(viewportsAdded/2.0)+1)*heightOfViewport);
	
		//Ogre::Viewport* detailVP = window->addViewport(camera_P,1,0,0.8,0.2,0.2);
		Ogre::Viewport* tmp = window->addViewport(camera_P,zOrdering,x,y,widthOfViewport,heightOfViewport);
		//Ogre::Viewport* tmp = window->addViewport(camera_P,zOrdering,0,0.8,0.2,0.2);
		//Ogre::Viewport* tmp = window->addViewport(camera_P,zOrdering,0,0,widthOfViewport,heightOfViewport);
		//tmp->setDimensions(x,0.6,widthOfViewport,heightOfViewport); //You have to move it afterwards not sure why
		tmp->setOverlaysEnabled(false);
		camera_P->setAspectRatio(Ogre::Real(tmp->getActualWidth()) / Ogre::Real(tmp->getActualHeight()));
		viewportsAdded++; //Keep track of how many we've added
		
	
		return tmp;
	}
}

bool ViewportManager::declareNewPlayer(Player* player)
{
	return false;
}
