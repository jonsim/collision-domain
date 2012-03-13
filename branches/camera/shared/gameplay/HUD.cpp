/**
* @file		HUD.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"

HUD::HUD()
{
	wodOverlay = NULL;
	wodContainer = NULL;
	wodTextArea = NULL;
}

void HUD::initialize()
{
	this->setupWOD();
}

void HUD::setupWOD()
{
	OutputDebugString("Starting the HUD");
	
	//Creat the overlay and the container
	wodOverlay = 
		Ogre::OverlayManager::getSingleton().create( "WOD_OVERLAY" );
	wodOverlay->setZOrder(601);
	wodContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "WOD_CONTAINER" ) );
	wodOverlay->add2D(wodContainer);
	wodContainer->setPosition(0.4f,0.9f);

	//Create a textarea
	wodTextArea = Ogre::OverlayManager::getSingleton().
		createOverlayElement("TextArea","WOD_ELEMENT");

	wodTextArea->setDimensions(1.0,1.0);
	wodTextArea->setMetricsMode(Ogre::GMM_PIXELS);
	wodTextArea->setPosition(0,0);
	
	wodTextArea->setParameter("font_name","StarWars");
	wodTextArea->setParameter("char_height", "16");
	wodTextArea->setColour(Ogre::ColourValue::Red);

	
	wodTextArea->hide();
	wodContainer->addChild(wodTextArea);	
	wodOverlay->show();
}

void HUD::drawDeathMessage(Player* killer, Player* killed)
{
	if(wodTextArea == NULL)
		initialize();

	wodTextArea->setCaption("KILLING MESSAGE");
	wodTextArea->show();
}