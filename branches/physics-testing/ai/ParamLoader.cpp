#include "ParamLoader.h"
#include "tinyxml.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

ParamLoader::ParamLoader(string fileName)
{
	TiXmlDocument doc(fileName.c_str());
	TiXmlElement *pRoot, *pParam;
	bool fileLoaded = doc.LoadFile();

	if(!fileLoaded)
	{
		fprintf(stderr, "Failed to open AI parameters file\n");
		exit(1);
	}

	//get the root node
	pRoot = doc.FirstChildElement("Parameters");
	if(pRoot)
	{
		pParam = pRoot->FirstChildElement("Parameter");
		while(pParam)
		{
			string name = pParam->Attribute("name");
			if(!name.compare("MinDetectionBoxLength"))
				MinDetectionBoxLength = atof(pParam->Attribute("value"));
			else if(!name.compare("ObstacleAvoidWeight"))
				ObstacleAvoidanceWeight = atof(pParam->Attribute("value"));
			else if(!name.compare("WanderWeight"))
				WanderWeight = atof(pParam->Attribute("value"));
			else if(!name.compare("WallAvoidanceWeight"))
				WallAvoidanceWeight = atof(pParam->Attribute("value"));
			else if(!name.compare("SeekWeight"))
				SeekWeight = atof(pParam->Attribute("value"));
			else if(!name.compare("FleeWeight"))
				FleeWeight = atof(pParam->Attribute("value"));
			else if(!name.compare("PursuitWeight"))
				PursuitWeight = atof(pParam->Attribute("value"));
			else if(!name.compare("ViewDistance"))
				ViewDistance = atof(pParam->Attribute("value"));
			else if(!name.compare("WallDetectionFeelerLength"))
				WallDetectionFeelerLength = atof(pParam->Attribute("value"));
			else if(!name.compare("prWallAvoidance"))
				prWallAvoidance = atof(pParam->Attribute("value"));
			else if(!name.compare("prObstacleAvoidance"))
				prObstacleAvoidance = atof(pParam->Attribute("value"));
			else if(!name.compare("prWander"))
				prWander = atof(pParam->Attribute("value"));
			else if(!name.compare("prSeek"))
				prSeek = atof(pParam->Attribute("value"));
			else if(!name.compare("prFlee"))
				prFlee = atof(pParam->Attribute("value"));
			else if(!name.compare("NumSamplesForSmoothing"))
				NumSamplesForSmoothing = atof(pParam->Attribute("value"));
			else if(!name.compare("MinObstacleRadius"))
				MinObstacleRadius = atof(pParam->Attribute("value"));
			else if(!name.compare("MaxObstacleRadius"))
				MaxObstacleRadius = atof(pParam->Attribute("value"));
			else if(!name.compare("WanderDistance"))
				WanderDistance = atof(pParam->Attribute("value"));
			else if(!name.compare("WanderJitter"))
				WanderJitter = atof(pParam->Attribute("value"));
			else if(!name.compare("WanderRadius"))
				WanderRadius = atof(pParam->Attribute("value"));
			else if(!name.compare("MaxForce"))
				MaxForce = atof(pParam->Attribute("value"));

			//go to next element
			pParam = pParam->NextSiblingElement("Parameter");
		}
	}


}
