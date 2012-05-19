/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef SHAREDINCLUDES_H
#define SHAREDINCLUDES_H

#define INITIAL_HEALTH 500

enum PHYS_SHAPE : int
{
    PHYS_SHAPE_COLOSSEUM,
    PHYS_SHAPE_FOREST,
    PHYS_SHAPE_QUARRY,
    PHYS_SHAPE_CARPARK,
    PHYS_SHAPE_BANGER,
    PHYS_SHAPE_SMALLCAR,
    PHYS_SHAPE_TRUCK,
    PHYS_SHAPE_BANGER_DOOR,
    PHYS_SHAPE_BANGER_FBUMPER,
    PHYS_SHAPE_BANGER_RBUMPER,
    PHYS_SHAPE_SMALLCAR_DOOR,
    PHYS_SHAPE_SMALLCAR_FBUMPER,
    PHYS_SHAPE_SMALLCAR_RBUMPER,
    PHYS_SHAPE_SMALLCAR_HEADLIGHT,
    PHYS_SHAPE_TRUCK_DOOR,
    PHYS_SHAPE_TRUCK_RBUMPER,
    PHYS_SHAPE_TRUCK_WINGMIRROR,
    PHYS_SHAPE_POWERUP,

    PHYS_SHAPE_COUNT,
};

enum PowerupType
{
	POWERUP_HEALTH,
	POWERUP_MASS,
	POWERUP_SPEED,
	POWERUP_COUNT
};

#endif // #ifndef SHAREDINCLUDES_H
