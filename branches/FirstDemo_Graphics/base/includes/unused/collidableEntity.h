#ifndef COLLIDABLEENTITY_H
#define COLLIDABLEENTITY_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../graphics/includes/stdafx.h"
#include "drawableEntity.h"



/******************** CLASS DEFINITIONS ********************/
class collidableEntity : public drawableEntity
{
 
public:
    collidableEntity (void);
    ~collidableEntity (void);
};

#endif // #ifndef COLLIDABLEENTITY_H
