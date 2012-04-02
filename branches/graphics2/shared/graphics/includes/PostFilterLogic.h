/**
 * @file    PostFilterLogic.h
 * @brief   Handles the logic and associated listeners attached to the post filter compositors.
 */

#ifndef POSTFILTERLOGIC_H
#define POSTFILTERLOGIC_H



/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"

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
class HDRLogic : public ListenerFactoryLogic
{
private:
	Ogre::CompositorInstance::Listener* mListener;
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
public:
	void setBloomSize (float n);
};

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

class MotionBlurLogic : public ListenerFactoryLogic
{
private:
	Ogre::CompositorInstance::Listener* mListener;
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
public:
	void setBlurStrength (float n);
};



/*-------------------- COMPOSITOR LISTENERS --------------------*/
class HDRListener : public Ogre::CompositorInstance::Listener
{
friend HDRLogic;
private:
	int mVpWidth, mVpHeight;
	int bloomSize;
	float textureWeights[15][4];
	float textureHOffsets[15][4];
	float textureVOffsets[15][4];
	Ogre::GpuProgramParametersSharedPtr fpParams;
public:
	HDRListener();
	~HDRListener();
	void notifyViewportSize(int width, int height);
	void notifyCompositor(Ogre::CompositorInstance* instance);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

class BloomListener : public Ogre::CompositorInstance::Listener
{
friend BloomLogic;
private:
	float blurWeight;
	float originalWeight;
public:
	BloomListener();
	~BloomListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
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

class MotionBlurListener : public Ogre::CompositorInstance::Listener
{
friend MotionBlurLogic;
private:
	float blurStrength;
public:
	MotionBlurListener();
	~MotionBlurListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

#endif