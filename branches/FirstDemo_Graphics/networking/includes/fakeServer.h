/*------------------------------------------------------------------------------
  File:     fakeServer.h
  Purpose:  
 ------------------------------------------------------------------------------*/
#ifndef FAKESERVER_H
#define FAKESERVER_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../../graphics/includes/stdafx.h"
#include "../../base/includes/player.h"
#include "../../base/includes/playerSnapshot.h"



/******************** CLASS DEFINITIONS ********************/
class fakeServer
{
public:
    fakeServer (void);
    ~fakeServer (void);
    int allocateClientID (void);
    void sendClientInformation (int clientID, playerSnapshot snapshot);
    player updateClientInformation (int clientID);

private:
	player playerList[1]; // which is actually an array until I can figure out c++ lists
    int nextClientID;
};

#endif // #ifndef FAKESERVER_H
