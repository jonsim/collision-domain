#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "common.h"
#include <Ogre.h>

using namespace Ogre;

class CollisionDetection
{
public:
    CollisionDetection(SceneManager* scene);
    ~CollisionDetection();
    //Raytracing to the polygon level
    bool RaycastFromPoint(const Vector3 &point, const Vector3 &normal, const int type, Vector3 &result, String &ent);
    //AABB Raytracing
    bool AABBRaycastFromPoint(const Vector3 &point, const Vector3 &normal, const int type, Vector3 &result, String &ent);

private:
    void GetMeshInformation(const Ogre::MeshPtr mesh,
                                size_t &vertex_count,
                                Ogre::Vector3* &vertices,
                                size_t &index_count,
                                unsigned long* &indices,
                                const Ogre::Vector3 &position,
                                const Ogre::Quaternion &orient,
                                const Ogre::Vector3 &scale);


    RaySceneQuery* m_rIntersectRay;
    SceneManager* mSceneMgr;
};

#endif
