#ifndef POSTFILTERLOGIC_H
#define POSTFILTERLOGIC_H


/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

//The simple types of compositor logics will all do the same thing -
//Attach a listener to the created compositor
class ListenerFactoryLogic : public Ogre::CompositorLogic
{
public:
	virtual void compositorInstanceCreated(Ogre::CompositorInstance* newInstance);
	virtual void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance);

protected:
	//This is the method that implementations will need to override
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance) = 0;

private:
	typedef std::map<Ogre::CompositorInstance*, Ogre::CompositorInstance::Listener*> ListenerMap;
	ListenerMap mListeners;
};



//The compositor logic for the bloom compositor
class BloomLogic : public ListenerFactoryLogic
{
protected:
	Ogre::CompositorInstance::Listener* mListener;
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
};

//The compositor logic for the gaussian blur compositor
class GaussianBlurLogic : public ListenerFactoryLogic
{
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
};




// listeners
class BloomListener : public Ogre::CompositorInstance::Listener
{
protected:
	float blurWeight;
	float originalWeight;
	Ogre::GpuProgramParametersSharedPtr fpParams;
public:
	BloomListener();
	virtual ~BloomListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

class GaussianListener : public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	GaussianListener();
	virtual ~GaussianListener();
	void notifyViewportSize(int width, int height);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

#endif