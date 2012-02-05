/**
 * @file	AudioCore.cpp
 * @brief 	An interface for firing off sounds
 */

#include "stdafx.h"
#include "SharedIncludes.h"

void Powerup::hide()
{
    mNode->setDebugDisplayEnabled( false );
    mNode->setVisible(false);
}
