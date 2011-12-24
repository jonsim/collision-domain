/**
 * @file	NetworkCore.h
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


void NetworkCore::frameEvent(InputState *inputSnapshot)
{
    // Called once every frame (each time controls are sampled)
    // Do with this data as you wish - bundle them off in a little packet of joy to the server
}


CarSnapshot* NetworkCore::getCarSnapshotIfExistsSincePreviousGet(int playerID)
{
    return NULL;
}