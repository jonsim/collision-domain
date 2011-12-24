#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../../graphics/includes/stdafx.h"
#include "collidableEntity.h"



/******************** CLASS DEFINITIONS ********************/
class physicsEntity : public collidableEntity
{
 
public:
    physicsEntity (void);
    ~physicsEntity (void);
};

#endif // #ifndef PHYSICSENTITY_H
