/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
#include <sstream>
#include <math.h>

/*
	Used this for research - http://www.ogre3d.org/tikiwiki/Simple+Text+Output&structure=Cookbook
*/

ScoreBoard::ScoreBoard()
{
	//This fixes a bug in Ogre where it does not load fonts
	//http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59197
	//Took me quite a while to track down this stupid bug
	Ogre::ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) { iter.getNext()->load(); }
	
	isInitialized = false;
}

void ScoreBoard::show()
{
	//Make sure it's been setup
	if(!isInitialized)
		this->initialize();

	this->update();

	sbOverlay->show();

	this->isShown = true;
}

void ScoreBoard::hide()
{
	//Make sure it's been setup
	if(!isInitialized)
		this->initialize();

	if(isShown) {
		this->sbOverlay->hide();
		this->isShown = false;
	}
}

void ScoreBoard::update()
{
	this->textAreaT1->setCaption(this->buildScoreText());
}

void ScoreBoard::initialize()
{
	OutputDebugString("Starting the scoreboard");
	
	//Creat the overlay and the container
	sbOverlay = 
		Ogre::OverlayManager::getSingleton().create( "SCOREBOARD_OVERLAY" );
	sbOverlay->setZOrder(601);
	sbContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "SCOREBOARD_CONTAINER" ) );
	sbOverlay->add2D(sbContainer);
	sbContainer->setPosition(0.0f,0.0f);
	sbContainer->setMaterialName("ConstructionScreen");
		
	//Create a textarea
	Ogre::OverlayElement *textAreaHeader = 
			Ogre::OverlayManager::getSingleton().
				createOverlayElement("TextArea","SCOREBOARD_HEADER");
	textAreaHeader->setDimensions(0.9,0.3);
	textAreaHeader->setMetricsMode(Ogre::GMM_PIXELS);
	textAreaHeader->setPosition(40,40);
	textAreaHeader->setParameter("font_name","StarWars");
	textAreaHeader->setParameter("char_height", "30");
	textAreaHeader->setColour(Ogre::ColourValue::White);
	textAreaHeader->setCaption("Team 1 \t\t\t\t\t\t\t Team 2");
	sbContainer->addChild(textAreaHeader);

	this->textAreaT1 = Ogre::OverlayManager::getSingleton().
		createOverlayElement("TextArea","SCOREBOARD_ELEMENT");

	int screenWidth = GameCore::mGraphicsApplication->getMainViewPort()->getActualWidth();
	int screenHeight = GameCore::mGraphicsApplication->getMainViewPort()->getActualHeight();

	this->textAreaT1->setDimensions(0.9,0.6);
	this->textAreaT1->setMetricsMode(Ogre::GMM_PIXELS);
	this->textAreaT1->setPosition(40,100);
	
	this->textAreaT1->setParameter("font_name","StarWars");
	this->textAreaT1->setParameter("char_height", "15");
	this->textAreaT1->setColour(Ogre::ColourValue::White);

	this->textAreaT1->setCaption(this->buildScoreText());
	sbContainer->addChild(this->textAreaT1);	
	sbOverlay->hide();
	
	isShown = false;
	isInitialized = true;
}

std::string ScoreBoard::buildScoreText()
{
	std::stringstream buildingStream;

	//Can only do the scoreboard like this
	/*
	if(GameCore::mGameplay->numberOfTeams == 2)
	{
		Team* team0 = GameCore::mGameplay->getTeam(0);
		Team* team1 = GameCore::mGameplay->getTeam(1);

		buildingStream << "TN: " << GameCore::mGameplay->numberOfTeams << "\n";
		buildingStream << "T0: " << team0->getTeamSize() << "\n";
		buildingStream << "T1: " << team1->getTeamSize() << "\n";
	}
	*/
	int numberOfPlayers = GameCore::mPlayerPool->getNumberOfPlayers();
	//We only want the best so drop the rest
	if(numberOfPlayers > 10)
		numberOfPlayers = 10;

	//Print out all the players names
	for(int i=0;i<numberOfPlayers;i++)
	{
		Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
		buildingStream << tmpPlayer->getNickname() << " - " << tmpPlayer->getRoundScore() << "\n";
	}

	return buildingStream.str();
}