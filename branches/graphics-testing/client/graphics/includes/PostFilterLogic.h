/**
 * @file    PostFilterLogic.h
 * @brief   Handles the logic and associated listeners attached to the post filter compositors.
 */

#ifndef POSTFILTERLOGIC_H
#define POSTFILTERLOGIC_H



/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

// The basic compositor logics will all do the same thing: attach a listener to the created
// compositor. Define a shared definition to keep things minimal.
class ListenerFactoryLogic : public Ogre::CompositorLogic
{
public:
	virtual void compositorInstanceCreated(Ogre::CompositorInstance* newInstance);
	virtual void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance);

protected:
	// This is the method that implementations will need to override.
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance) = 0;

private:
	typedef std::map<Ogre::CompositorInstance*, Ogre::CompositorInstance::Listener*> ListenerMap;
	ListenerMap mListeners;
};



/*-------------------- COMPOSITOR LOGICS --------------------*/
class BloomLogic : public ListenerFactoryLogic
{
private:
	Ogre::CompositorInstance::Listener* mListener;
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
public:
	void setBlurWeight (float n);
	void setOriginalWeight (float n);
};

class RadialBlurLogic : public ListenerFactoryLogic
{
private:
	Ogre::CompositorInstance::Listener* mListener;
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
public:
	void setBlurDistance (float n);
	void setBlurStrength (float n);
};



/*-------------------- COMPOSITOR LISTENERS --------------------*/
class BloomListener : public Ogre::CompositorInstance::Listener
{
friend BloomLogic;
private:
	float blurWeight;
	float originalWeight;
	Ogre::GpuProgramParametersSharedPtr fpParams;
public:
	BloomListener();
	~BloomListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

class RadialBlurListener : public Ogre::CompositorInstance::Listener
{
friend RadialBlurLogic;
private:
	float blurDistance;
	float blurStrength;
	Ogre::GpuProgramParametersSharedPtr fpParams;
public:
	RadialBlurListener();
	~RadialBlurListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

#endif