/**
* @file        Gameplay.cpp
* @brief    Manages the view ports for a window
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

    //Create a textarea
    Ogre::OverlayElement *textArea = 
        Ogre::OverlayManager::getSingleton().
            createOverlayElement("TextArea","SCOREBOARD_ELEMENT");

    textArea->setDimensions(1.0,1.0);
    textArea->setMetricsMode(Ogre::GMM_PIXELS);
    textArea->setPosition(0,0);
    
    textArea->setParameter("font_name","StarWars");
    textArea->setParameter("char_height", "42");
    textArea->setColour(Ogre::ColourValue::Green);

    textArea->setCaption(this->buildScoreText());
    textArea->show();
    sbContainer->addChild(textArea);    
    sbOverlay->show();
}

std::string ScoreBoard::buildScoreText()
{
    std::stringstream buildingStream;

    //Header
    buildingStream << "Score Board\n\n";
    buildingStream << "Team 1 \t\t\t Team 2\n\n";

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
        buildingStream << tmpPlayer->getNickname() << "\n";
    }

    return buildingStream.str();
}