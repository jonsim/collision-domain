/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "ScoreBoard.h"
#include "GameCore.h"
#include <sstream>
#include <math.h>

#define STRIP_HEIGHT 0.05f

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
	isForced = false;
}

void ScoreBoard::show()
{
	//Make sure it's been setup
	if(!isInitialized)
		this->initialize();

    this->manageStrips();
	this->update();

	sbOverlay->show();
    textScoreOverlay->show();

	this->isShown = true;
}

void ScoreBoard::showForce()
{
	//Make sure it's been setup
	if(!isInitialized)
		this->initialize();
    this->manageStrips();
	this->update();

	sbOverlay->show();
    textScoreOverlay->show();

	this->isShown = true;
	this->isForced = true;
}


void ScoreBoard::hideForce()
{
	//Make sure it's been setup
	if(!isInitialized)
		this->initialize();

	if(isShown) {
		this->sbOverlay->hide();
        textScoreOverlay->hide();
		this->isShown = false;
		this->isForced = false;
	}
}

void ScoreBoard::hide()
{
	//Make sure it's been setup
	if(!isInitialized)
		this->initialize();

	if(isShown && !isForced) {
		this->sbOverlay->hide();
        textScoreOverlay->hide();
		this->isShown = false;
	}
}

void ScoreBoard::update()
{
	this->textAreaT1->setCaption(this->buildScoreText(BLUE_TEAM));
    this->textAreaT2->setCaption(this->buildScoreText(RED_TEAM));
}

void ScoreBoard::initialize()
{
#ifdef COLLISION_DOMAIN_CLIENT
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
		
    textScoreOverlay =
        Ogre::OverlayManager::getSingleton().create( "SCOREBOARD_TEXT_OVERLAY" );
	textScoreOverlay->setZOrder(602);
	Ogre::OverlayContainer *textScoreContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "SCOREBOARD_TEXT_CONTAINER" ) );
	textScoreOverlay->add2D(textScoreContainer);
	textScoreContainer->setPosition(0.0f,0.0f);

	//Create a textarea
	Ogre::OverlayElement *textAreaHeader = 
			Ogre::OverlayManager::getSingleton().
				createOverlayElement("Panel","SCOREBOARD_HEADER");
    textAreaHeader->setMetricsMode(Ogre::GMM_RELATIVE);
	textAreaHeader->setDimensions(1.0f, 0.2f);
	textAreaHeader->setPosition(0,0);
    textAreaHeader->setMaterialName("TeamBanner");
	sbContainer->addChild(textAreaHeader);


    int screenWidth = GameCore::mClientGraphics->screenWidth;
    int screenHeight = GameCore::mClientGraphics->screenHeight;

    int t1X = (50.0/800.0)*(float)screenWidth;
    int t2X = (460.0/800.0)*(float)screenWidth;

    int tY = (95.0/600.0)*(float)screenHeight;

	this->textAreaT1 = Ogre::OverlayManager::getSingleton().
		createOverlayElement("TextArea","ZSCOREBOARD_ELEMENT1");
    this->textAreaT1->setDimensions(0.9f, 0.6f);
	this->textAreaT1->setMetricsMode(Ogre::GMM_PIXELS);
	this->textAreaT1->setPosition(t1X,tY);
	
	this->textAreaT1->setParameter("font_name","DejaVuSans");
	this->textAreaT1->setParameter("char_height", "30");
	this->textAreaT1->setColour(Ogre::ColourValue::White);
	this->textAreaT1->setCaption(this->buildScoreText(BLUE_TEAM));
	textScoreContainer->addChild(this->textAreaT1);	

    this->textAreaT2 = Ogre::OverlayManager::getSingleton().
		createOverlayElement("TextArea","ZSCOREBOARD_ELEMENT2");
    this->textAreaT2->setDimensions(0.9f, 0.6f);
	this->textAreaT2->setMetricsMode(Ogre::GMM_PIXELS);
	this->textAreaT2->setPosition(t2X,tY);
	
	this->textAreaT2->setParameter("font_name","DejaVuSans");
	this->textAreaT2->setParameter("char_height", "30");
	this->textAreaT2->setColour(Ogre::ColourValue::White);

	this->textAreaT2->setCaption(this->buildScoreText(RED_TEAM));
	textScoreContainer->addChild(this->textAreaT2);	

	sbOverlay->hide();
	
	isShown = false;
	isInitialized = true;
#endif
}

void ScoreBoard::manageStrips()
{
    int numRedPlayers = 0;
    int numBluePlayers = 0;

    std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();
    for(int i=0;i<players.size();i++)
    {
        switch(players[i]->getTeam())
        {
            case BLUE_TEAM:
                numBluePlayers++;
                break;
            case RED_TEAM:
                numRedPlayers++;
                break;
        }
    }



    StringStream tmpSS2;
    tmpSS2 << "Blue: "<<numBluePlayers<< " Red: " <<numRedPlayers<<"\n";
    OutputDebugString(tmpSS2.str().c_str());

    //Make sure we've got the correct number of strip overlays
    if(blueTeamStrips.size() != numBluePlayers)
    {
        for(int i=blueTeamStrips.size()-1;i<numBluePlayers-1;i++) {
            StringStream tmpSS;
            tmpSS << "OLE_BLUESTRIP__"<<i;

	        Ogre::OverlayElement *tmp = 
			        Ogre::OverlayManager::getSingleton().
                        createOverlayElement("Panel",tmpSS.str());
	        tmp->setMetricsMode(Ogre::GMM_RELATIVE);
            tmp->setDimensions(0.4f, STRIP_HEIGHT);
            float yOffeset = STRIP_HEIGHT*i+0.2;
            tmp->setPosition(0.05f,yOffeset);
            if((i%2) == 0) 
                tmp->setMaterialName("BlueStripDark");
            else
                tmp->setMaterialName("BlueStripLight");
	        sbContainer->addChild(tmp);
            blueTeamStrips.push_back(tmp);

        }
    }

    if(redTeamStrips.size() != numRedPlayers)
    {
        for(int i=redTeamStrips.size()-1;i<numRedPlayers-1;i++) {
            StringStream tmpSS;
            tmpSS << "OLE_REDSTRIP__"<<i;

	        Ogre::OverlayElement *tmp = 
			        Ogre::OverlayManager::getSingleton().
                        createOverlayElement("Panel",tmpSS.str());
	        tmp->setMetricsMode(Ogre::GMM_RELATIVE);
            tmp->setDimensions(0.4f, STRIP_HEIGHT);
            float yOffeset = STRIP_HEIGHT*i+0.2;
            tmp->setPosition(0.55f,yOffeset);
            if((i%2) == 0) 
                tmp->setMaterialName("RedStripDark");
            else
                tmp->setMaterialName("RedStripLight");
	        sbContainer->addChild(tmp);
            redTeamStrips.push_back(tmp);
        }
    }
}

std::string ScoreBoard::buildScoreText(TeamID teamID)
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

	//#ifdef COLLISION_DOMAIN_SERVER
		//Get the sorted list
		std::vector<Player*> sortedPlayers = GameCore::mPlayerPool->getScoreOrderedPlayers();
		//Print out all the players names
		//for(int i=0;i<numberOfPlayers;i++)
		for(int i=(sortedPlayers.size()-1);i>=0;i--)
		{
			//Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
			Player* tmpPlayer = sortedPlayers[i];
            if(tmpPlayer->getTeam() == teamID)
    			buildingStream << tmpPlayer->getNickname() << " - " << tmpPlayer->getRoundScore() << "\n";
		}
	//#endif

	return buildingStream.str();
}
