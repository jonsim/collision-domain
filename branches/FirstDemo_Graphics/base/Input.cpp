/*------------------------------------------------------------------------------
  File:     playerSnapshot.cpp
  Purpose:  
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../graphics/includes/stdafx.h"
#include "includes/playerSnapshot.h"



/******************** METHOD DEFINITIONS ********************/

/*------------------------------------------------------------------------------
  Method:       
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Constructor.
 ------------------------------------------------------------------------------*/
playerSnapshot::playerSnapshot (playerKeyState forward, playerKeyState turn)
{
    forwardState = forward;
    turnState = turn;
}


/*------------------------------------------------------------------------------
  Method:       
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Deconstructor.
 ------------------------------------------------------------------------------*/
playerSnapshot::~playerSnapshot (void)
{
}


playerKeyState playerSnapshot::revealForward (void)
{
    return forwardState;
}


playerKeyState playerSnapshot::revealTurn (void)
{
    return turnState;
}