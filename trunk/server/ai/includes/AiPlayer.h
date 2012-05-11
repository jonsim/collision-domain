#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <string>
#include "SteeringBehaviour.h"
#include "utils.h"
#include "RakNetTypes.h"

using namespace std;
using namespace Ogre;

class SteeringBehaviour;
class Player;
enum CarType;

#define NOTABLE_CHANGE_RATIO 10.0f
#define TIME_BEFORE_STUCK 100
#define TIME_BEFORE_UNSTUCK 100

enum level
{
	easy,
	normal,
	hard
};

class AiPlayer
{
public:
	AiPlayer(string name, Ogre::Vector3 startPos, Ogre::SceneManager* sceneManager, int flags, level diff);
	AiPlayer() {};
	~AiPlayer() {};

    void Spawn();
	void Update(double timeSinceLastFrame);
	Vector3 GetFleeTarget() { return mFleeTarget; };
	Vector3 GetSeekTarget() { return mSeekTarget; };
	Vector3 GetPos();
	float GetMaxSpeed() { return mMaxSpeed; };
	Vector3 GetVelocity() { return mVelocity; };
	Quaternion GetHeading();
	string GetName() { return mName; };
	Vector3 getFeelerPos() { return mFeelerPosition; };
	Vector3 getWallHitPosition(void)const{return mWallHitPosition;}
    Vector3 getWallNormal(void)const{return mWallNormal;}
    std::vector<Vector3> getFeelersPosition(void)const{ return mFeelers;}
    SteeringBehaviour* getSteeringBehaviour(){return mSteeringBehaviour;}

private:
	string mName;
	Player* mPlayer;
	Vector3 mFleeTarget;
	Vector3 mSeekTarget;
	float mTimeElapsed;
	std::vector<Vector3> feelers;
	float mWallDetectionFeelerLength;
	Vector3 mWallHitPosition;
	Vector3 mFeelerPosition;
	Vector3 mWallNormal;
	float mMaxSpeed;
	Vector3 mVelocity;
	SteeringBehaviour* mSteeringBehaviour;
	RakNet::Packet* mPacket;
	CarType mCarType;
	double mTolerance;
	std::vector<Vector3> mFeelers;
	void CreateFeelers();
    Vector3 FindWalls();
	double mFeelerDectionLength;
	int turn, direction;
	double targetDistance;
	level difficulty;


    //Stuck detection
    void isStuck();
    void updateStuckDetection();
    Vector3 oldPosition;
    int timeSinceNotableChange; // This is the number of cycles since a notable change in positino
    int stuckMode;//0 = No, 1 = Go back, 2= Go Back to normal
    int timeInStuckMode; 
    int timeInChangeOver;
};

#endif
