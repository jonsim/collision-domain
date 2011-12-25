/**
 * @file	NetworkCore.h
 * @brief 	Takes notifications and deals with them however networking needs to
 */
#ifndef NETWORKCORE_H
#define NETWORKCORE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "NetworkCore.h"
#include "InputState.h"
#include "CarSnapshot.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief       A class to take notifications and deal with them however networking needs to
 *
 */
class NetworkCore
{
public:
    NetworkCore (void);
    ~NetworkCore (void);
    void frameEvent(InputState *inputSnapshot);
    CarSnapshot* getCarSnapshotIfExistsSincePreviousGet(int playerID);
};

#endif // #ifndef NETWORKCORE_H
