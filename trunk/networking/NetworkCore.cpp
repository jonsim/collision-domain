/**
 * @file	NetworkCore.cpp
 * @brief 	Takes notifications and deal with them however networking needs to
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "NetworkCore.h"



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, initialising all resources.
NetworkCore::NetworkCore()
{
    // Fire off a new raknet thread in here to do the networking.
}


/// @brief  Deconstructor.
NetworkCore::~NetworkCore()
{

}


/// @brief  Called once a frame with the latest user keypresses.
/// @param  inputSnapshot  The object containing the latest user keypresses.
void NetworkCore::frameEvent(InputState *inputSnapshot)
{
    // Called once every frame (each time controls are sampled)
    // Do with this data as you wish - bundle them off in a little packet of joy to the server
}


/// @brief  Called once a frame and supplies the requested user position so it can be updated onscreen.
/// @param  playerID  An identifier for a specific player in the game.
/// @return NULL if no CarSnapshot has been received since the last time this method was polled.
///         The latest CarSnapshot for the requested player if one has been received (or more than one :)).
CarSnapshot* NetworkCore::getCarSnapshotIfExistsSincePreviousGet(int playerID)
{
    return NULL;
}
