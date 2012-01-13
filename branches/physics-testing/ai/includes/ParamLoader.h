//ParamLoader class
#ifndef PARAMLOADER_H
#define PARAMLOADER_H

#include <string>

using namespace std;

class ParamLoader
{
public:
	double MinDetectionBoxLength;
	double ObstacleAvoidanceWeight;
	double WanderWeight;
	double WallAvoidanceWeight;
	double ViewDistance;
	double WallDetectionFeelerLength;
	double WanderDistance;
	double WanderJitter;
	double WanderRadius;
	double SeekWeight;
	double FleeWeight;
	double PursuitWeight;
	double prWallAvoidance;
	double prObstacleAvoidance;
	double prWander;
	double prSeek;
	double prFlee;
	int    NumSamplesForSmoothing;
	int    MinObstacleRadius;
	int    MaxObstacleRadius;
	double MaxForce;


	//ParamLoader(){};
	~ParamLoader() {};
	ParamLoader(string fileName);
};

#endif
