#ifndef BASEGAMEENTITY_H
#define BASEGAMEENTITY_H

#include <cassert>
#include <vector>
#include <string>

#include <OgreVector3.h>

struct Telegram;

class BaseGameEntity
{
  //Variables
  public:
    enum {default_entity_type = -1};
  private:
    int m_ID; ///Each entity has a unique ID
    int m_EntityType; ///Every entity has a type associated with it
    bool m_bTag; ///A generic flag
  protected:
    Ogre::Vector3 m_vPos; ///It's location in the environment
    Ogre::Vector3 m_vScale;
    double m_dBoundingRadius; ///The length of this object's bounding radius
  //Functions
  protected:
    BaseGameEntity();
    BaseGameEntity(int entity_type);
    BaseGameEntity(int entity_type, Ogre::Vector3 pos, double r);
    BaseGameEntity(int entity_type, int ForcedID);
  public:
    virtual ~BaseGameEntity();
    virtual void Update(double time_elapsed){};
    virtual bool HandleMessage(const Telegram& msg){return false;}

    /**@brief This way, the entities can write to their data stream */
    virtual void Write(std::ostream& os)const{}
    /**@brief This way, the entities can read to their data stream */
    virtual void Read(std::ifstream& is){}

    //Accessors
    Ogre::Vector3 Pos(void)const{return m_vPos;}
    void          SetPos(Ogre::Vector3 new_pos){m_vPos = new_pos;}

    inline double BRadius(void)const{return m_dBoundingRadius;}
    inline void SetBRadius(double r){m_dBoundingRadius = r;}
    inline int ID(void)const{ return m_ID;}

    inline bool IsTagged(void)const{return m_bTag;}
    inline void Tag(void){m_bTag = true;}
    inline void UnTag(void){m_bTag = false;}

    Ogre::Vector3 Scale(void)const{return m_vScale;}
    void SetScale(Ogre::Vector3 val);
    void SetScale(double val);

    int EntityType(void)const{return m_EntityType;}
    void SetEntityType(int new_type){m_EntityType = new_type;}

  private:
    /**@brief Used by the constructor to ensure a unique ID */
    int NextValidID(){static int NextID = 0; return NextID++;}
};

#endif // BASEGAMEENTITY_H
