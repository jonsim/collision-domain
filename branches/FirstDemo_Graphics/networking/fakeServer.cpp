/*------------------------------------------------------------------------------
  File:     fakeServer.cpp
  Purpose:  
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../graphics/includes/stdafx.h"
#include "includes/fakeServer.h"
#include "../base/includes/player.h"
#include "../base/includes/playerSnapshot.h"



/******************** CONSTANT DEFINITIONS ********************/
static const Ogre::Real playerTurningConstant = 0.6;       // The amount that the player turns when cornering (?? / sec)
static const Ogre::Real playerAccelerationConstant = 800;  // The amount that the player linearly accelerates by when moving forward (units / sec^2)
static const Ogre::Real playerTopSpeed = 1000;             // The maximum speed of the player (units / sec)
static const Ogre::Real playerFrictionConstant = 0.4;      // The amount of energy a player loses per second to friction (percentage)



/******************** METHOD DEFINITIONS ********************/

/*------------------------------------------------------------------------------
  Method:       fakeServer::fakeServer (void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Constructor.
 ------------------------------------------------------------------------------*/
fakeServer::fakeServer (void)
{
    nextClientID = 0;
}


/*------------------------------------------------------------------------------
  Method:       fakeServer::~fakeServer (void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Deconstructor.
 ------------------------------------------------------------------------------*/
fakeServer::~fakeServer (void)
{
}


int fakeServer::allocateClientID (void)
{
    return nextClientID++;
}

void fakeServer::sendClientInformation (int clientID, playerSnapshot snapshot)
{
    //playerList[clientID].updateSpeed(clientSpeed);
    //playerList[clientID].updateAccel(clientAccel);
    //playerList[clientID].calculateState(snapshot);
}

player fakeServer::updateClientInformation (int clientID)
{
    return playerList[clientID];
}