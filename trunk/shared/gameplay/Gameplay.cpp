/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Gameplay.h"
#include "GameCore.h"
#include <sstream>
#include <math.h>

Gameplay::Gameplay() : mGameActive(false), mCountDownActive(false)
{
    mTeams[0] = new Team(BLUE_TEAM);
    mTeams[1] = new Team(RED_TEAM);
    mSB = new ScoreBoard();
    startTime = time(NULL);
    roundNumber = -1;
    wtInitalised = false;    
}


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
    /*
	if(mGameMode == VIP_MODE) {
		vipModeGameWon();
	}
    */
	return false;
}

bool Gameplay::hasWon(TeamID teamID)
{
	return false;
}

void Gameplay::generateGameOrder (void)
{
#ifdef COLLISION_DOMAIN_SERVER
    char chosenOrderString[64];

    // Generate games
    sprintf(chosenOrderString, "Gamemode order: ");
    for (int i = 0; i < 3; i++)
    {
        bool exitCondition = false;
        int rInt;
        GameMode rMode;

        while (!exitCondition)
        {
            rInt  = rand() % 3;
            rMode = (rInt == 0) ? FFA_MODE : ((rInt == 1) ? TDM_MODE : VIP_MODE);
            exitCondition = true;
            for (int j = 0; j < i; j++)
                if (mGamemodeOrder[j] == rMode)
                    exitCondition = false;
        }
        mGamemodeOrder[i] = rMode;
        if (rMode == FFA_MODE)
            sprintf(chosenOrderString, "%s Free for All,", chosenOrderString);
        else if (rMode == TDM_MODE)
            sprintf(chosenOrderString, "%s Team Deathmatch,", chosenOrderString);
        else
            sprintf(chosenOrderString, "%s VIP Mode,", chosenOrderString);
    }
    chosenOrderString[strlen(chosenOrderString)-1] = '\n';
    GameCore::mGui->outputToConsole(chosenOrderString);


    // Generate arenas
    sprintf(chosenOrderString, "Arena order: ");
    for (int i = 0; i < 3; i++)
    {
        bool exitCondition = false;
        int rInt;
        ArenaID rArena;
        while (!exitCondition)
        {
            rInt  = rand() % 3;
            rArena = (rInt == 0) ? COLOSSEUM_ARENA : ((rInt == 1) ? FOREST_ARENA : QUARRY_ARENA);
            exitCondition = true;
            for (int j = 0; j < i; j++)
                if (mArenaOrder[j] == rArena)
                    exitCondition = false;
        }
        mArenaOrder[i] = rArena;
        if (rArena == COLOSSEUM_ARENA)
            sprintf(chosenOrderString, "%s Colosseum,", chosenOrderString);
        else if (rArena == FOREST_ARENA)
            sprintf(chosenOrderString, "%s Forest,", chosenOrderString);
        else
            sprintf(chosenOrderString, "%s Quarry,", chosenOrderString);
    }
    chosenOrderString[strlen(chosenOrderString)-1] = '\n';
    GameCore::mGui->outputToConsole(chosenOrderString);
#endif
}

void Gameplay::cycleGame( bool unload )
{
#ifdef COLLISION_DOMAIN_SERVER
    // Next round plz.
    roundNumber++;

    if(roundNumber > 2)
    {
        roundNumber = 0;
        this->generateGameOrder();
    }

    StringStream tmpSS;
    tmpSS << "Round Number: " << roundNumber << "\n";
    GameCore::mGui->outputToConsole(tmpSS.str().c_str());

    // Get the next gamemode.
    this->setGameMode(mGamemodeOrder[roundNumber]);
    
    // Get the next arena.
    if(unload == true && roundNumber > 0)
        GameCore::mServerGraphics->unloadArena(mArenaOrder[roundNumber-1]);

    this->setArenaID(mArenaOrder[roundNumber]);

    GameCore::mServerGraphics->loadArena(mArenaOrder[roundNumber]);

    // Sync those bad boiz up
    GameCore::mNetworkCore->sendGameSync(mGamemodeOrder[roundNumber], mArenaOrder[roundNumber]);
#endif
}

void Gameplay::setGameMode(GameMode gameMode)
{
	mGameMode = gameMode;
}

void Gameplay::handleNewRound()
{
    #ifdef COLLISION_DOMAIN_SERVER
        //this->cycleGameMode(); //Move onto the next game type;
    #endif  
    //this->incrementRoundNumber();
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

bool Gameplay::addPlayer(RakNet::RakNetGUID playerid, TeamID requestedTeam)
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

    return true;
}

void Gameplay::playerQuit(Player *player)
{
    TeamID tid = (TeamID) player->getTeam();
    if( tid == NO_TEAM )
        return;
    Team *t = getTeam(tid);
#ifdef COLLISION_DOMAIN_SERVER
    while( t->getVIP() == player )
        setNewVIP(tid);    
#endif
    t->delPlayer(player);
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
    //OutputDebugString(tmpOutputString.str().c_str());
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

void Gameplay::spawnPlayers()
{
#ifdef COLLISION_DOMAIN_SERVER
    int size = GameCore::mPlayerPool->getNumberOfPlayers();

	for(int i=0;i<size;i++)
	{
		Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
        if( tmpPlayer->getPlayerState() == PLAYER_STATE_WAIT_SPAWN )
        {
            GameCore::mNetworkCore->sendPlayerSpawn( tmpPlayer );
        }
    }
#endif
}

void Gameplay::positionPlayers()
{
	int totalNumberOfPlayers = GameCore::mPlayerPool->getNumberOfPlayers();
	int hypo = 40; //The hypotonuse. Increase to spread out
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
			//OutputDebugString(debugString.str().c_str());
			
			if(tmpPlayer->getCar() != NULL)
				tmpPlayer->getCar()->moveTo(btVector3(x,1,y));
		}
	}	
}

void Gameplay::startGame()
{
    if(!mCountDownActive)
    {
        mCountDownActive = true; //Not strictly true but should fix things
        //Spawn the start new round thing
        InfoItem* newRoundII = new InfoItem(NEW_ROUND_OT, 1000, 3000);
	    mInfoItems.push_back(newRoundII);
	    this->calculateRoundScores();
        #ifdef COLLISION_DOMAIN_SERVER
            GameCore::mGui->outputToConsole("Game started.\n");
        #endif
    }

    this->spawnPlayers();
	this->positionPlayers();
	
    //Only set a VIP if we're in VIP mode
    if(this->getGameMode() == VIP_MODE)
    {
        this->setNewVIPs();
    }
    //this->scheduleCountDown();
	//mGameActive = true;
}

void Gameplay::drawInfo()
{
	//Loop through all the info items
	std::vector<InfoItem*>::iterator itr;
	for(itr = mInfoItems.begin(); itr<mInfoItems.end(); ++itr)
	{
		InfoItem* tmpInfoItem = *itr;
		//If GameTime is after the start of the item
		if(RakNet::GreaterThan(RakNet::GetTime(),tmpInfoItem->getStartTime()))
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
			// This stops it crashing as we've changed the underlyign vector it's accessing.
            // Is not a problem really as it gets called every frame.
            // Also ensures not to much work gets done per frame.
            break;
            
		}
	}
}

void Gameplay::handleInfoItem(InfoItem* item, bool show)
{
    item->sendPacket(show); //Send the packet, will only happen on the server
    
	#ifdef COLLISION_DOMAIN_CLIENT
		Ogre::OverlayElement* tmpOLE = Ogre::OverlayManager::getSingleton().getOverlayElement("ONE_OT",false);
	#endif
	#ifdef COLLISION_DOMAIN_SERVER
		InfoItem* transitionII;
        StringStream tmpSS;
    #endif

	if(show)
	{
		switch(item->getOverlayType())
		{
			case ONE_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
                    tmpOLE->hide();
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "countdown_1" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
                    OutputDebugString("ONE!\n");
                    #ifdef COLLISION_DOMAIN_SERVER
                        this->startTime = time(NULL);
                    #endif
                    if( GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
                        GameCore::mClientGraphics->mBigScreen->resetRoundTimer();
                    if( GameCore::mPlayerPool->getLocalPlayer()->getPlayerState() == PLAYER_STATE_SPECTATE )
                        GameCore::mPlayerPool->spectateNext();
				#endif
                #ifdef COLLISION_DOMAIN_SERVER
                    GameCore::mGui->outputToConsole("One!\n");
                #endif
				break;
			case TWO_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
                    tmpOLE->hide();
					tmpOLE->setDimensions(0.1f,0.1f);
					tmpOLE->setMaterialName( "countdown_2" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
                    OutputDebugString("Two!\n");
				#endif
                #ifdef COLLISION_DOMAIN_SERVER
                    GameCore::mGui->outputToConsole("Two!\n");
                #endif
				break;
			case THREE_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
                    tmpOLE->hide();
                    this->hideGameTypeText();
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "countdown_3" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
                    OutputDebugString("Three!\n");
				#endif
                #ifdef COLLISION_DOMAIN_SERVER
                    GameCore::mGui->outputToConsole("Three!\n");
                #endif
				break;
			case FOUR_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
                    tmpOLE->hide();
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "gear4" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
                    OutputDebugString("Four!\n");
				#endif
                #ifdef COLLISION_DOMAIN_SERVER
                    GameCore::mGui->outputToConsole("Four!\n");
                #endif
				break;
			case FIVE_OT:
				#ifdef COLLISION_DOMAIN_CLIENT
                    tmpOLE->hide();
					tmpOLE->setDimensions(0.1f, 0.1f);
					tmpOLE->setMaterialName( "gear5" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
                    OutputDebugString("Five!\n");
				#endif
                #ifdef COLLISION_DOMAIN_SERVER
                    GameCore::mGui->outputToConsole("Five!\n");
                #endif
				break;

            case GO_OT:
                #ifdef COLLISION_DOMAIN_CLIENT
                    tmpOLE->hide();
					tmpOLE->setDimensions(0.1f,0.1f);
					tmpOLE->setMaterialName( "countdown_go" );
					tmpOLE->setPosition(0.45f, 0.1f);
					tmpOLE->show();
                    // put the rock music back
                    GameCore::mAudioCore->menuToRockTrack(false);
                #endif

                mGameActive = true;
                mCountDownActive = false;
                break;

			case ROUND_OVER_OT:
                // put the menu music back
                #ifdef COLLISION_DOMAIN_CLIENT
                    GameCore::mAudioCore->menuToRockTrack(true);
                #endif

                mGameActive = false;
                //this->cycleGameMode(); //Cycle game mode
				#ifdef COLLISION_DOMAIN_CLIENT
					mSB->showForce();
				#endif

				#ifdef COLLISION_DOMAIN_SERVER
					GameCore::mGui->outputToConsole("Round Ended.\n");

                    //Show the wining player II
					transitionII = new InfoItem(SCOREBOARD_TO_WINNER_OT, 5000, 100);
					mInfoItems.push_back(transitionII);

				#endif
				break;
			case SCOREBOARD_TO_WINNER_OT:
                //Hide the scoreboard
                #ifdef COLLISION_DOMAIN_CLIENT
					mSB->hideForce();
					if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)						
						GameCore::mClientGraphics->mBigScreen->hideScreen();         
                    this->showWinnerText(this->getRoundWinner(),true);

                    //GameCore::mClientGraphics->addPodium(Ogre::Vector3(0,1,0));
                #else
                    //Show the wining player II
					transitionII = new InfoItem(CLEAR_PODIUM_OT, 5000, 100);
					mInfoItems.push_back(transitionII);
                #endif
                break;
            case CLEAR_PODIUM_OT:
                #ifdef COLLISION_DOMAIN_CLIENT
                    //GameCore::mClientGraphics->removePodium();
                #endif

                //Reset the gmae
                this->mGameActive = false;
                this->restartGame();
                cycleGame();
                GameCore::mPlayerPool->roundEnd();

                break;
			case NEW_ROUND_OT:
                this->handleNewRound();
				#ifdef COLLISION_DOMAIN_CLIENT
                    this->hideWinnerText();
                    this->showGameTypeText();
					if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR) {
						//Move the camera back
						//GameCore::mClientGraphics->mCamera->setPosition(Ogre::Vector3(0,10,0));
						//GameCore::mClientGraphics->mCamera->lookAt(Ogre::Vector3(0,100,0));
						//GameCore::mClientGraphics->mCamera->setNearClipDistance(5);
						GameCore::mClientGraphics->mBigScreen->showScreen();
					}
				#endif
				#ifdef COLLISION_DOMAIN_SERVER
                    
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

void Gameplay::showGameTypeText()
{
    if(!wtInitalised)
        this->createWinnerTextOverlay();

    this->textAreaT1->setCaption("");
    if(this->getGameMode() == FFA_MODE)
    {
        this->oleGameMode->setMaterialName("GameModeFFA");
        //this->textAreaT1->setCaption("FREE FOR ALL!!");
    }
    else if(this->getGameMode() == TDM_MODE)
    {
        this->oleGameMode->setMaterialName("GameModeTDM");
        //this->textAreaT1->setCaption("TEAM DEATHMATCH");
    }
    else if(this->getGameMode() == VIP_MODE)
    {
        this->oleGameMode->setMaterialName("GameModeVIP");
        //this->textAreaT1->setCaption("GET THE VIPS");
    }
    else
    {
        //OutputDebugString("Not a recognised game mode");
    }

    this->oleGameMode->show();
    this->wtOverlay->show();
}

void Gameplay::hideGameTypeText()
{
    if(!wtInitalised)
        this->createWinnerTextOverlay();
    
    //Hid the game mode overaly
    this->oleGameMode->hide();

    if(this->wtOverlay->isVisible())
        this->wtOverlay->hide();
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
    if(!wtInitalised)
        this->createWinnerTextOverlay();
    
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
	
    //Create the GameMode Overlay
    this->oleGameMode = Ogre::OverlayManager::getSingleton().
		createOverlayElement("Panel","GAME_MODE_ELEMENT");
	this->oleGameMode->setPosition(0.0f,0.0f);
    this->oleGameMode->setMaterialName("GameModeFFA");
	this->wtContainer->addChild(oleGameMode);
    
    //Finish intialising
    this->wtOverlay->hide();
    this->wtInitalised = true;

    
}

void Gameplay::scheduleCountDown()
{
	#ifdef COLLISION_DOMAIN_SERVER
        mCountDownActive = true;
		GameCore::mGui->outputToConsole("Scheduling countdown.\n");
		InfoItem* threeII = new InfoItem(THREE_OT, 5000, 900);
		InfoItem* twoII = new InfoItem(TWO_OT, 6000, 900);
		InfoItem* oneII = new InfoItem(ONE_OT, 7000, 900);
        InfoItem* goII = new InfoItem(GO_OT,8000,900);

		mInfoItems.push_back(threeII);
		mInfoItems.push_back(twoII);
		mInfoItems.push_back(oneII);
        mInfoItems.push_back(goII);

		//Countdown Timer
		InfoItem* fiveEII = new InfoItem(FIVE_OT,184000,900);
		InfoItem* fourEII = new InfoItem(FOUR_OT,185000,900);
		InfoItem* threeEII = new InfoItem(THREE_OT,186000,900);
		InfoItem* twoEII = new InfoItem(TWO_OT,187000,900);
		InfoItem* oneEII = new InfoItem(ONE_OT,188000,900);

		mInfoItems.push_back(fiveEII);
		mInfoItems.push_back(fourEII);
		mInfoItems.push_back(threeEII);
		mInfoItems.push_back(twoEII);
		mInfoItems.push_back(oneEII);

		//Round over
		InfoItem* roEII = new InfoItem(ROUND_OVER_OT,189000,2900);
		mInfoItems.push_back(roEII);
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

/* This handles the various scoring modes dependent on the gameplay
    mode
*/
void Gameplay::handleDeath(Player* deadPlayer, Player* causedBy)
{
#ifdef COLLISION_DOMAIN_SERVER
    StringStream tmpSS;
    int redTeamAlive = 0;
    int blueTeamAlive = 0;
    int totalAlive = 0;

    switch(this->mGameMode)
    {
        case FFA_MODE:
            causedBy->addToScore(1);
            break;
        case TDM_MODE:
            if(causedBy->getTeam() != deadPlayer->getTeam())
            {
                //If they're on different teams
                causedBy->addToScore(1);
            }
            else
            {
                //If they're on the same team (BAD!!)
                causedBy->addToScore(-1); //Deduct!!!!
            }
            
            //Check to see if team is all dead
            for(int i=0;i<GameCore::mPlayerPool->getNumberOfPlayers();i++)
            {
                Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
                if(tmpPlayer->getAlive())
                {
                    totalAlive++;
                    if(tmpPlayer->getTeam() == 1)
                    {
                        blueTeamAlive++;
                    }
                    else
                    {
                        redTeamAlive++;
                    }
                }
            }

            
            //tmpSS << "Red Team Alive: " << redTeamAlive << " Blue team alive: " << blueTeamAlive << "\n";
            //GameCore::mGui->outputToConsole(tmpSS.str().c_str());
            if(totalAlive <= 1 || redTeamAlive == 0 || blueTeamAlive == 0)
                this->forceRoundEnd();

            break;
        case VIP_MODE:
            //In this game mode we're going to finish the round if a VIP is killed
            //Please note normal score calculations still apply
            if(causedBy->getTeam() != deadPlayer->getTeam())
            {
                //If they're on different teams
                causedBy->addToScore(1);
                if(deadPlayer->getVIP())
                {
                    causedBy->addToGameScore(3); //Give him some score for winning
                    //Now add 1 to all players on that team
                    std::vector<Player*> tmpPlayers = GameCore::mPlayerPool->getPlayers();
                    for(unsigned int i=0;i<tmpPlayers.size();i++)
                    {
                        if(tmpPlayers[i]->getTeam() == causedBy->getTeam())
                        {
                            tmpPlayers[i]->addToGameScore(1);
                        }
                    }
                    this->forceRoundEnd();
                    break;
                }
            }
            else
            {
                //If they're on the same team (BAD!!)
                causedBy->addToScore(-1);
                if(deadPlayer->getVIP())
                {
                    causedBy->addToGameScore(-5); //THIS IS REALLY BAD SO WE'LL HURT THEIR GAME SCORE
                    this->forceRoundEnd();
                    break;
                }
            }

             //Check to see if all players are dead
            for(int i=0;i<GameCore::mPlayerPool->getNumberOfPlayers();i++)
            {
                Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
                if(tmpPlayer->getAlive())
                {
                    totalAlive++;
                }
            }
            if(totalAlive <= 1)
                this->forceRoundEnd();
            break;
        default:
            //This is here just in case of a bug...
            //Shouldn't ever get called but better than no game
            //OutputDebugString("No game mode detected!!\n");
            causedBy->addToScore(1);
    }
    GameCore::mNetworkCore->sendSyncScores();
#endif
}

std::vector<DEATH*> Gameplay::getDeathList()
{
	return this->deathList;
}

void Gameplay::restartGame()
{
	// Loop through all players setting health to full
	this->resetAllHP();
    this->resetRoundScores();
}

void Gameplay::resetRoundScores() {
    std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();

    for(unsigned int i=0;i<players.size();i++)
    {
        players[i]->setRoundScore(0);
    }
}

//This has been rewritten into a much nicer form
void Gameplay::calculateRoundScores()
{
    std::vector<Player*> players = GameCore::mPlayerPool->getScoreOrderedPlayers();
    for(int i=(int)(players.size()-1);i>=(int)(players.size()-NUM_TOP_PLAYERS-1);i--)
	{
        //If there are not at NUM_TOP_PLAYERS this will save it from breaking
        if(i<0)
            break;
		players[i]->addToGameScore(NUM_TOP_PLAYERS-i+1);
	}
}

// This method removes any InfoItems left over relating to this round and
// Pushes a new round end in 1 seconds
void Gameplay::forceRoundEnd()
{
    //Remove all existing round items
    this->mInfoItems.clear();

    //Spawn new round end
	this->mInfoItems.push_back(new InfoItem(ROUND_OVER_OT,1000,2900));
}
