/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
#include <sstream>
#include <math.h>



Gameplay::Gameplay() : mGameActive(false)
{
	mSB = new ScoreBoard();
	//mHUD = new HUD();
	//this->setNumberOfTeams(2); //Might as well default it to 2
    mTeams[0] = new Team(BLUE_TEAM);
    mTeams[1] = new Team(RED_TEAM);

    //Set initialised variables
    wtInitalised = false;    
}

/*Ogre::Real Gameplay::getScorePercentage(std::string identifier)
{
	return 0.0;
}

int	Gameplay::getScoreValue(std::string identifier)
{
	return 0;
}*/

Team* Gameplay::getTeam(TeamID teamID)
{
    switch (teamID)
    {
        case BLUE_TEAM: return mTeams[0]; break;
        case RED_TEAM:  return mTeams[1]; break;
        case NO_TEAM:   OutputDebugString("getTeam called with team NO_TEAM. IM GONNA GO RIGHT AHEAD AND CRASH NOW LOL.\n");
        default:        throw Ogre::Exception::ERR_INVALIDPARAMS; break;
    }
}

bool Gameplay::gameOver()
{
	if(mGameMode == VIP_MODE) {
		vipModeGameWon();
	}
	return false;
}

bool Gameplay::hasWon(TeamID teamID)
{
	return false;
}

void Gameplay::setGameMode(GameMode gameMode)
{
	mGameMode = gameMode;
}

bool Gameplay::vipModeGameWon()
{
	return false;
}

void Gameplay::setNewVIP(TeamID teamID)
{
    Team* t = getTeam(teamID);
    t->setNewVIP(t->getRandomPlayer());
    
        /*// Manage and assign VIP Cameras for the server
#ifdef COLLISION_DOMAIN_SERVER
	if(team == teams[0])
		pPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam1);
	else if(team == teams[1])
		pPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam2);
#endif*/
}
    
void Gameplay::setNewVIP(TeamID teamID, Player* newVIP)
{
    getTeam(teamID)->setNewVIP(newVIP);
}

void Gameplay::setNewVIPs()
{
    setNewVIP(BLUE_TEAM);
    setNewVIP(RED_TEAM);
//	std::vector<Team*>::iterator itr;
//	for(itr = teams.begin(); itr<teams.end(); ++itr)
//	{
		//Team* team = *itr;
		//Player* vipPlayer = team->getRandomPlayer();
        
/*        // Manage and assign VIP Cameras for the server
#ifdef COLLISION_DOMAIN_SERVER
		//Clear the previous assignments
		GameCore::mGraphicsApplication->mViewCam1->detachFromParent();
		GameCore::mGraphicsApplication->mViewCam2->detachFromParent();
		//Assign to the new VIPS
		if(team == teams[0])
			vipPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam1);
		else if(team == teams[1])
			vipPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam2);
#endif*/
		//team->setNewVIP(vipPlayer);
	//}
}

bool Gameplay::addPlayer( RakNet::RakNetGUID playerid, TeamID requestedTeam )
{
    Team* teamToJoin;
    Player* pPlayer = GameCore::mPlayerPool->getPlayer(playerid);

    // If the requested team number is invalid autoassign the team, otherwise check 
    // the team choice and join the team if possible or report an error.
    if (requestedTeam == NO_TEAM)
    {
        teamToJoin = autoAssignTeam();
    }
    else
    {
        if (validateTeamChoice(requestedTeam))
            teamToJoin = getTeam(requestedTeam);
        else
            return false;
    }
    
    // Join the team.
    teamToJoin->addPlayer(pPlayer);

    //Check to see if we need to start game
    if(this->mGameActive == false && GameCore::mPlayerPool->getNumberOfPlayers() >= NUM_PLAYERS_TO_START)
    {
        this->startGame();
    }

    return true;
}
 
/*TeamID Gameplay::autoAssignTeam()
{
#if NUM_TEAMS == 2
    if (mTeams[0]->getTeamSize() > mTeams[1]->getTeamSize())
        return RED_TEAM;
    return BLUE_TEAM;
#else
    #error "Code not rewritten for !2 teams."
#endif
}*/

bool Gameplay::validateTeamChoice(TeamID requestedTeam)
{
#if NUM_TEAMS != 2
    #error "Code not written for !2 teams."
#endif
    TeamID otherTeam = (requestedTeam == BLUE_TEAM) ? RED_TEAM : BLUE_TEAM;

    if (getTeam(requestedTeam)->getTeamSize() > getTeam(otherTeam)->getTeamSize())
        return false;
    return true;
}

/*void Gameplay::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer   = GameCore::mPlayerPool->getPlayer(playerid);
	Team*   teamToJoin  = autoAssignTeam();
	teamToJoin->addPlayer(tmpPlayer);

	//Check to see if we need to start game
	if(this->mGameActive == false && GameCore::mPlayerPool->getNumberOfPlayers() >= NUM_PLAYERS_TO_START)
	{
		this->startGame();
	}
}*/

Team* Gameplay::autoAssignTeam()
{
#if NUM_TEAMS == 2
    if (mTeams[0]->getTeamSize() > mTeams[1]->getTeamSize())
        return mTeams[1];
    return mTeams[0];
#else
    #error "Code not rewritten for !2 teams."
#endif
}
/*
//Gets which team makes sense to join (Aims to balance)
Team* Gameplay::getTeamToJoin()
{
	Team*	lowestTeam;
	int		lowestNumOfPlayers;

	//Check to see that there is some teams to join
	if(teams.size() > 0)
	{
		//Set initial size to an actual value. Who knows it might be the correct value
		lowestTeam = teams[0];
		lowestNumOfPlayers = lowestTeam->getTeamSize();

		//Loop through all teams and find the one with the lowest value
		std::vector<Team*>::iterator itr;
		for(itr = teams.begin(); itr<teams.end(); ++itr)
		{
			Team* team = *itr;
			if(team->getTeamSize() < lowestNumOfPlayers)
			{
				lowestTeam = team;
				lowestNumOfPlayers = team->getTeamSize();
			}
		}
	}

	return lowestTeam;
}*/

void Gameplay::notifyDamage(Player* player)
{
	this->printTeamStats();
}

void Gameplay::printTeamStats()
{
    std::stringstream tmpOutputString;
    tmpOutputString << "Team Stats:\n";
#if NUM_TEAMS == 2
    tmpOutputString << "  Blue Team: " << getTeam(BLUE_TEAM)->getTotalTeamHP() << " health\n";
    tmpOutputString << "  Red Team: " << getTeam(RED_TEAM)->getTotalTeamHP() << " health\n";
#else
    for (int i = 0; i < NUM_TEAMS; i++)
        tmpOutputString << "  teams[" << i << "]: " << teams[i]->getTotalTeamHP() << " health\n";
#endif
    OutputDebugString(tmpOutputString.str().c_str());
}

/*Team* Gameplay::checkIfGameOver()
{
	Team* winningTeam = NULL;
	//Loop through 
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{
		Team* tmpTeam = *itr;
		if(tmpTeam->getTotalTeamHP() > 0)
		{
			//If we already had a winning team it means we didn't as there are two
			if(winningTeam != NULL)
			{
				return NULL;
			}
			else
			{
				winningTeam = tmpTeam;
			}
		}
	}

	return winningTeam;
}*/

void Gameplay::preparePlayers()
{
	resetAllHP();
	positionPlayers();
}

void Gameplay::resetAllHP()
{
	int size = GameCore::mPlayerPool->getNumberOfPlayers();
	//Loop through the entire array
	for(int i=0;i<size;i++)
	{
		Player* player = GameCore::mPlayerPool->getPlayer(i);
		if(player != NULL)
		{
			player->resetHP();
		}
	}
}

void Gameplay::positionPlayers()
{
	int totalNumberOfPlayers = GameCore::mPlayerPool->getNumberOfPlayers();
	int hypo = 25; //The hypotonuse. Increase to spread out
	//Calculate segment angle
	Ogre::Real segSize = (2*Ogre::Math::PI)/totalNumberOfPlayers;
	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	for(int i=0;i<size;i++)
	{
		Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
		if(tmpPlayer != NULL)
		{
			//Calcualte the correct positions
			Ogre::Real omega = (Ogre::Real)i*segSize;
			//Calculate which sector of the circle it's in
			int sector = floor(omega/ (Ogre::Math::PI/2));
			//Adjust to keep omega under 90
			omega-=(Ogre::Math::PI/2)*sector;

			Ogre::Real x = 0.0;
			Ogre::Real y = 0.0;

			switch(sector)
			{
				case 0:
					y = cos(omega)*hypo;
					x = sin(omega)*hypo;
					break;
				case 1:
					x = cos(omega)*hypo;
					y = -sin(omega)*hypo;
					break;
				case 2:
					y = -cos(omega)*hypo;
					x = -sin(omega)*hypo;
					break;
				case 3:
					x = -cos(omega)*hypo;
					y = sin(omega)*hypo;
					break;
			}

			//TODO - Move cars to correct positions
			std::stringstream debugString;
			debugString << "Alignment Car Pos X: "<< x << " Y: "<<y<<"\n";
			OutputDebugString(debugString.str().c_str());
			
			if(tmpPlayer->getCar() != NULL)
				tmpPlayer->getCar()->moveTo(btVector3(x,1,y));
		}
	}	
}

void Gameplay::startGame()
{
	this->positionPlayers();
	this->setNewVIPs(); //TODO - Change this once we have multiple game modes
	this->scheduleCountDown();
	mGameActive = true;
	#ifdef COLLISION_DOMAIN_SERVER
		GameCore::mGui->outputToConsole("Game started.\n");
	#endif
}

void Gameplay::drawInfo()
{
	//Loop through all the info items
	std::vector<InfoItem*>::iterator itr;
	for(itr = mInfoItems.begin(); itr<mInfoItems.end(); ++itr)
	{
		InfoItem* tmpInfoItem = *itr;
		//If the start time of the thing is less that the current time
		if(RakNet::LessThan(tmpInfoItem->getStartTime(),RakNet::GetTime()))
		{
			if(!tmpInfoItem->getDrawn())
			{
				handleInfoItem(tmpInfoItem,true);
				tmpInfoItem->setDrawn();
				//mInfoItems.erase(itr);
			}
			else
			{
				if(RakNet::GreaterThan(RakNet::GetTime(),tmpInfoItem->getEndTime()))
				{
					handleInfoItem(tmpInfoItem,false);
					mInfoItems.erase(itr);
					delete tmpInfoItem;
				}
			}
			break; //Can really only manage one a turn
		}
	}
}

void Gameplay::handleInfoItem(InfoItem* item, bool show)
{
	#ifdef COLLISION_DOMAIN_CLIENT
		Ogre::OverlayElement* tmpOLE = Ogre::OverlayManager::getSingleton().getOverlayElement("ONE_OT",false);
	#endif
	#ifdef COLLISION_DOMAIN_SERVER
	    InfoItem* newRoundII;
		InfoItem* transitionII;
    #endif

	if(show)
	{
		switch(item->getOverlayType())
		{
			case ONE_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "gear1" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
				#endif
				break;
			case TWO_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					tmpOLE->setDimensions(0.1f,0.1f);
					tmpOLE->setMaterialName( "gear2" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
				#endif
				break;
			case THREE_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "gear3" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
				#endif
				break;
			case FOUR_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "gear4" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
				#endif
				break;
			case FIVE_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "gear5" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
				#endif
				break;
			case ROUND_OVER_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					mSB->showForce();
				#endif

				#ifdef COLLISION_DOMAIN_SERVER
					GameCore::mGui->outputToConsole("Rounded Ended.\n");

                    //Show the wining player II
					transitionII = new InfoItem(SCOREBOARD_TO_WINNER_OT, 5000, 100);
					mInfoItems.push_back(transitionII);
					transitionII->sendPacket();

                    
                    if(this->getRoundNumber() < NUM_ROUNDS)
                    {
                        //New Round II
                        newRoundII = new InfoItem(NEW_ROUND_OT, 10000, 1000);
					    mInfoItems.push_back(newRoundII);
					    this->calculateRoundScores();
                        newRoundII->sendPacket();
                    }
                    else
                    {
                        //End of game II
                    }
				#endif
				break;
			case SCOREBOARD_TO_WINNER_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
					mSB->hideForce();
					if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR) {
						
						GameCore::mClientGraphics->mBigScreen->hideScreen();
						//Move the camera
						//GameCore::mClientGraphics->mCamera->setPosition(Ogre::Vector3(0,0,80));
						//GameCore::mClientGraphics->mCamera->setNearClipDistance(5);
						//GameCore::mClientGraphics->mCamera->lookAt(Ogre::Vector3(0,0,-300));
					}
                    

                    this->showWinnerText(this->getRoundWinner(),true);
				#endif
				#ifdef COLLISION_DOMAIN_SERVER

				#endif
				
				break;
			case NEW_ROUND_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
                    this->hideWinnerText();
					if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR) {
						//Move the camera back
						//GameCore::mClientGraphics->mCamera->setPosition(Ogre::Vector3(0,10,0));
						//GameCore::mClientGraphics->mCamera->lookAt(Ogre::Vector3(0,100,0));
						//GameCore::mClientGraphics->mCamera->setNearClipDistance(5);
						GameCore::mClientGraphics->mBigScreen->showScreen();
					}
				#endif
				#ifdef COLLISION_DOMAIN_SERVER
                    this->incrementRoundNumber();
					this->scheduleCountDown();
				#endif
				break;
		}
	}
	else
	{
		#ifdef COLLISION_DOMAIN_CLIENT
			tmpOLE->hide();
		#endif
	}
}

Player* Gameplay::getRoundWinner()
{
    std::vector<Player*> orderedPlayers = GameCore::mPlayerPool->getScoreOrderedPlayers();    
    return orderedPlayers[orderedPlayers.size()-1];
}

/* If round is set to true it does round winning screen if false it
   does end of game winning screen */
void Gameplay::showWinnerText(Player* winningPlayer, bool round)
{
    if(!wtInitalised)
        this->createWinnerTextOverlay();

    std::stringstream tmpString;
    tmpString << winningPlayer->getNickname() << " won the ";

    if(round)
    {
        tmpString << "round!!!";
    }
    else
    {
        tmpString << "game!!!";
    }
    
    this->textAreaT1->setCaption(tmpString.str());
    this->wtOverlay->show();
}

void Gameplay::hideWinnerText()
{
    this->wtOverlay->hide();
}

void Gameplay::createWinnerTextOverlay()
{
    wtOverlay = Ogre::OverlayManager::getSingleton().create( "WINNER_TEXT_OVERLAY" );
    wtOverlay->setZOrder(601);

    wtContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "WINNER_TEXT_CONTAINER" ) );
	wtOverlay->add2D(wtContainer);
	wtContainer->setPosition(0.0f,0.0f);

    //Text stuff
    this->textAreaT1 = Ogre::OverlayManager::getSingleton().
		createOverlayElement("TextArea","WINNER_TEXT_ELEMENT");

	this->textAreaT1->setDimensions(0.9f, 0.6f);
	this->textAreaT1->setMetricsMode(Ogre::GMM_PIXELS);
	this->textAreaT1->setPosition(100,100);
	
	this->textAreaT1->setParameter("font_name","DejaVuSans");
	this->textAreaT1->setParameter("char_height", "60");
	this->textAreaT1->setColour(Ogre::ColourValue::White);

	this->wtContainer->addChild(this->textAreaT1);
	this->wtOverlay->hide();
    this->wtInitalised = true;
}

void Gameplay::scheduleCountDown()
{
	#ifdef COLLISION_DOMAIN_SERVER
		GameCore::mGui->outputToConsole("Scheduling countdown.\n");
		InfoItem* threeII = new InfoItem(THREE_OT, 1000, 1000);
		InfoItem* twoII = new InfoItem(TWO_OT, 2000, 1000);
		InfoItem* oneII = new InfoItem(ONE_OT, 3000, 1000);


		mInfoItems.push_back(threeII);
		mInfoItems.push_back(twoII);
		mInfoItems.push_back(oneII);

		//Countdown Timer
		InfoItem* fiveEII = new InfoItem(FIVE_OT,115000,1000);
		InfoItem* fourEII = new InfoItem(FOUR_OT,116000,1000);
		InfoItem* threeEII = new InfoItem(THREE_OT,117000,1000);
		InfoItem* twoEII = new InfoItem(TWO_OT,118000,1000);
		InfoItem* oneEII = new InfoItem(ONE_OT,119000,1000);

		mInfoItems.push_back(fiveEII);
		mInfoItems.push_back(fourEII);
		mInfoItems.push_back(threeEII);
		mInfoItems.push_back(twoEII);
		mInfoItems.push_back(oneEII);

		//GAME OVER
		InfoItem* roEII = new InfoItem(ROUND_OVER_OT,120000,3000);
		mInfoItems.push_back(roEII);

		//Send packets
	
		threeII->sendPacket();
		twoII->sendPacket();
		oneII->sendPacket();
		fiveEII->sendPacket();
		fourEII->sendPacket();
		threeEII->sendPacket();
		twoEII->sendPacket();
		oneEII->sendPacket();
		roEII->sendPacket();
	#endif
}

void Gameplay::setupOverlay()
{
	Ogre::Overlay *olInfo = 
		Ogre::OverlayManager::getSingleton().create( "OVERLAY_INFO" );
	olInfo->setZOrder(600);
	olInfo->show();
	
	
	Ogre::OverlayContainer* olContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "INFOCONT" ) );
	//olContainer = olContainerTmp;

	olContainer->setMetricsMode( Ogre::GMM_RELATIVE );
	olContainer->setHorizontalAlignment( Ogre::GHA_LEFT);
	olContainer->setVerticalAlignment(Ogre::GVA_TOP);
	//Set the map
	olContainer->setDimensions(1.0f, 1.0f);
	olInfo->add2D(olContainer);
	
	Ogre::OverlayElement* tmpOLE = 
					Ogre::OverlayManager::getSingleton().createOverlayElement(
					"Panel",
					"ONE_OT");
			
	tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
	tmpOLE->setDimensions(0.1f, 0.1f);
	tmpOLE->setMaterialName( "gear3" );
	tmpOLE->setPosition(0.45f, 0.1f);
	tmpOLE->hide();
	olContainer->addChild(tmpOLE);
}

void Gameplay::drawDeathInfo()
{
	mHUD->drawDeathMessage(NULL,NULL);
}

void Gameplay::initialize()
{
	mHUD->initialize();
}

void Gameplay::markDeath(Player* deadPlayer, Player* causedBy)
{
	DEATH newDeath;
	newDeath.player = deadPlayer;
	newDeath.causedBy = causedBy;
	deathList.push_back(&newDeath);
}

std::vector<DEATH*> Gameplay::getDeathList()
{
	return this->deathList;
}

void Gameplay::restartGame()
{
	// Loop through all players setting health to full
	this->resetAllHP();
	// Set game to not active
	this->mGameActive = false;
}

void Gameplay::calculateRoundScores()
{
	for(int j=0;j<5;j++)
		topPlayers[j] = NULL;

	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	//Loop through each of the players in the game
	for(int i=0;i<size;i++)
	{
		//Only consider them if they're not NULL
		Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
		if(tmpPlayer != NULL)
		{
			//Loop through each of the top players inserting the new player in the correct place
			for(int j=0;j<NUM_TOP_PLAYERS;j++)
			{
				//If they're is no player in this slot put it in
				if(topPlayers[j] == NULL)
				{
					topPlayers[j] = tmpPlayer;
					break;
				}
				else if(tmpPlayer->getRoundScore() >= topPlayers[j]->getRoundScore())
				{
					//IN the case where the socre is better than or comperable
					//TODO - Do some randomness here it choosed either player some times
					//Rotate the palyers array
					for(int z=(NUM_TOP_PLAYERS-1);z>j;z--)
					{
						topPlayers[z] = topPlayers[z-1];
					}
					topPlayers[j] = tmpPlayer;

				}
			}
		}
	}

	for(int i=0;i<NUM_TOP_PLAYERS;i++)
	{
		topPlayers[i]->addToGameScore(NUM_TOP_PLAYERS-i+1);
		
	}
}
