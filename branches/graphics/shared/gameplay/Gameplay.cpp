/**
* @file        Gameplay.cpp
* @brief    Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include <sstream>
#include <math.h>



/*Gameplay::Gameplay() : mGameActive(false)
{
    //mSB = new ScoreBoard();
    //mHUD = new HUD();
}*/

void Gameplay::createTeams()
{
    // As it is pretty much guarenteed we will have 2 players, a safer method is provided for
    // creating them, however a general purpose method is also available.
#if NUM_TEAMS == 2
    teams[0] = new Team(BLUE_TEAM);
    teams[1] = new Team(RED_TEAM);
#else
    for (int i = 0; i < NUM_TEAMS; i++)
        teams[i] = new Team(i+1);
#endif
}
/*
float Gameplay::getScorePercentage(int teamNumber)
{
    return 0.0;
}

int Gameplay::getScoreValue(int teamNumber)
{
    return 0;
}

void Gameplay::gameOver()
{
    if(mGameplayMode == VIP_MODE) {
        vipModeGameWon();
    }
}

bool Gameplay::hasWon(Team* team)
{
    return false;
}

Team* Gameplay::checkIfGameOver()
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

bool Gameplay::vipModeGameWon()
{
    std::vector<Team*>::iterator itr;
    for(itr = teams.begin(); itr<teams.end(); ++itr)
    {

    }
    return false;
}
}*/

void Gameplay::setNewVIP(TeamID id)
{
    if (id == NO_TEAM)
        return;
    Team* team = getTeam(id);
    team->setVIP(team->getRandomPlayer());
    
    // Manage and assign VIP Cameras for the server
/*#ifdef COLLISION_DOMAIN_SERVER
    if(team == teams[0])
        pPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam1);
    else if(team == teams[1])
        pPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam2);
#endif*/
}

void Gameplay::setNewVIPs()
{
    // As it is pretty much guarenteed we will have 2 players, a safer method is provided for
    // accessing them, however a general purpose method is also available.
#if NUM_TEAMS == 2
    setNewVIP(BLUE_TEAM);
    setNewVIP(RED_TEAM);
#else
    for (int i = 1; i <= NUM_TEAMS; i++)
        setNewVIP((TeamID) i);
#endif
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

Team* Gameplay::autoAssignTeam()
{
#if NUM_TEAMS == 2
    if (teams[0]->getTeamSize() > teams[1]->getTeamSize())
        return teams[1];
    return teams[0];
#else
    #error "Code not updated for !2 teams."
    Team* lowestTeam;
    int   lowestNumOfPlayers;

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
#endif
}

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

void Gameplay::preparePlayers()
{
    resetAllHP();
    positionPlayers();
}

void Gameplay::resetAllHP()
{
    //Loop through the entire array
    for(int i=0;i<MAX_PLAYERS;i++)
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

    for(int i=0;i<MAX_PLAYERS;i++)
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
            //std::stringstream debugString;
            //debugString << "Alignment Car Pos X: "<< x << " Y: "<<y<<"\n";
            //OutputDebugString(debugString.str().c_str());
            tmpPlayer->getCar()->moveTo(btVector3(x,1,y));
        }
    }    
}

void Gameplay::startGame()
{
    startRound();
    mGameActive = true;
}

void Gameplay::startRound()
{
    positionPlayers();
    switch (mGamemode)
    {
        case VIP_MODE:
            setNewVIPs();
            break;
        default:
            OutputDebugString("startRound called with an unrecognised gamemode.\n");
            break;
    }
    scheduleCountDown();
}
/*
void Gameplay::drawInfo()
{
    std::vector<InfoItem*>::iterator itr;
    for(itr = mInfoItems.begin(); itr<mInfoItems.end(); ++itr)
    {
        InfoItem* tmpInfoItem = *itr;
        if(RakNet::LessThan(tmpInfoItem->getStartTime(),RakNet::GetTime()))
        {
            
            //if(tmpInfoItem->getDrawn())
            //{
                //if(RakNet::LessThan(tmpInfoItem->getEndTime(),RakNet::GetTime()))
                //{
                    //handleInfoItem(tmpInfoItem,false);
                    //mInfoItems.erase(itr);
                //}
            //}
            //else
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
            break;
        }
    }
}

void Gameplay::handleInfoItem(InfoItem* item, bool show)
{
    Ogre::OverlayElement* tmpOLE = Ogre::OverlayManager::getSingleton().getOverlayElement("ONE_OT",false);
    #ifdef COLLISION_DOMAIN_SERVER
        InfoItem* newGameII;
    #endif

    if(show)
    {
        switch(item->getOverlayType())
        {
            case ONE_OT:
                tmpOLE->setDimensions(0.1f, 0.1f);
                tmpOLE->setMaterialName( "gear1" );
                tmpOLE->setPosition(0.45f, 0.1f);
                tmpOLE->show();
                break;
            case TWO_OT:
                tmpOLE->setDimensions(0.1f,0.1f);
                tmpOLE->setMaterialName( "gear2" );
                tmpOLE->setPosition(0.45f, 0.1f);
                tmpOLE->show();
                break;
            case THREE_OT:
                tmpOLE->setDimensions(0.1f, 0.1f);
                tmpOLE->setMaterialName( "gear3" );
                tmpOLE->setPosition(0.45f, 0.1f);
                tmpOLE->show();
                break;
            case FOUR_OT:
                tmpOLE->setDimensions(0.1f, 0.1f);
                tmpOLE->setMaterialName( "gear4" );
                tmpOLE->setPosition(0.45f, 0.1f);
                tmpOLE->show();
                break;
            case FIVE_OT:
                tmpOLE->setDimensions(0.1f, 0.1f);
                tmpOLE->setMaterialName( "gear5" );
                tmpOLE->setPosition(0.45f, 0.1f);
                tmpOLE->show();
                break;
            case GAME_OVER_OT:
                tmpOLE->setDimensions(0.1f, 0.1f);
                tmpOLE->setMaterialName( "map_top_1" );
                tmpOLE->setPosition(0.45f, 0.1f);
                tmpOLE->show();
                #ifdef COLLISION_DOMAIN_SERVER
                    newGameII = new InfoItem(NEW_GAME_OT, 5000, 1000);
                    mInfoItems.push_back(newGameII);
                    this->calculateRoundScores();
                #endif
                break;
            case NEW_GAME_OT:
                #ifdef COLLISION_DOMAIN_SERVER
                    this->scheduleCountDown();
                #endif
                break;
        }
    }
    else
    {
        tmpOLE->hide();
    }
}

void Gameplay::scheduleCountDown()
{
    #ifdef COLLISION_DOMAIN_SERVER
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
        InfoItem* goEII = new InfoItem(GAME_OVER_OT,120000,3000);
        mInfoItems.push_back(goEII);

        //Send packets
    
        threeII->sendPacket();
        twoII->sendPacket();
        oneII->sendPacket();
        fiveEII->sendPacket();
        fourEII->sendPacket();
        threeEII->sendPacket();
        twoEII->sendPacket();
        oneEII->sendPacket();
        goEII->sendPacket();
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

    //Loop through each of the players in the game
    for(int i=0;i<MAX_PLAYERS;i++)
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
*/