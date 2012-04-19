/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef SHAREDINCLUDES_H
#define SHAREDINCLUDES_H

enum PHYS_SHAPE : int
{
    PHYS_SHAPE_ARENA,
    PHYS_SHAPE_CARPARK,
    PHYS_SHAPE_QUARRY,
    PHYS_SHAPE_BANGER,
    PHYS_SHAPE_SMALLCAR,
    PHYS_SHAPE_TRUCK,
    PHYS_SHAPE_BANGER_DOOR,
    PHYS_SHAPE_BANGER_FBUMPER,
    PHYS_SHAPE_BANGER_RBUMPER,
    PHYS_SHAPE_SMALLCAR_DOOR,
    PHYS_SHAPE_SMALLCAR_FBUMPER,
    PHYS_SHAPE_SMALLCAR_RBUMPER,
    PHYS_SHAPE_TRUCK_DOOR,
    PHYS_SHAPE_TRUCK_RBUMPER,
    PHYS_SHAPE_POWERUP,

    PHYS_SHAPE_COUNT,
};

#include "MeshDeformer.h"
#include "AudioCore.h"
#include "InputState.h"
#include "Input.h"
#include "CarSnapshot.h"
#include "Vehicle.h"
#include "Car.h"
#include "PowerupPool.h"
#include "PlayerCollisions.h"
#include "PhysicsCore.h"
#include "cars/SimpleCoupeCar.h"
#include "cars/SmallCar.h"
#include "cars/TruckCar.h"
#include "PostFilterLogic.h"
#include "SceneSetup.h"
#include "ViewCamera.h"
#include "Camera.h"
#include "GameCore.h"
#include "ScoreBoard.h"
#include "HUD.h"
#include "Death.h"
#include "Gameplay.h"
#include "Team.h"
#include "InfoItem.h"


#endif // #ifndef SHAREDINCLUDES_H
