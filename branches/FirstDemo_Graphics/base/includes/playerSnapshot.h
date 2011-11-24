/*------------------------------------------------------------------------------
  File:     playerSnapshot.h
  Purpose:  
 ------------------------------------------------------------------------------*/
#ifndef PLAYERSNAPSHOT_H
#define PLAYERSNAPSHOT_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../../graphics/includes/stdafx.h"



enum playerKeyState {POSITIVE=1, OFF=0, NEGATIVE=-1};

/******************** CLASS DEFINITIONS ********************/
class playerSnapshot
{
public:
    playerSnapshot (playerKeyState forward, playerKeyState turn);
    ~playerSnapshot (void);
    playerKeyState revealForward (void);
    playerKeyState revealTurn (void);

private:
    playerKeyState forwardState;
    playerKeyState turnState;
};

#endif // #ifndef PLAYERSNAPSHOT_H
