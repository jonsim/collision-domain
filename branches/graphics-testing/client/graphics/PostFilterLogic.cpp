/**
 * @file    PostFilterLogic.h
 * @brief   Handles the logic and associated listeners attached to the post filter compositors.
 */



/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"



/*-------------------- LISTENER FACTORY LOGIC --------------------*/
/// @copydoc CompositorLogic::compositorInstanceCreated
void ListenerFactoryLogic::compositorInstanceCreated(Ogre::CompositorInstance* newInstance) 
{
	Ogre::CompositorInstance::Listener* listener = createListener(newInstance);
	newInstance->addListener(listener);
	mListeners[newInstance] = listener;
}

/// @copydoc CompositorLogic::compositorInstanceDestroyed
void ListenerFactoryLogic::compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance)
{
	delete mListeners[destroyedInstance];
	mListeners.erase(destroyedInstance);
}



/*-------------------- COMPOSITOR LOGICS --------------------*/
/*-------------------- BLOOM LOGIC --------------------*/
/// @copydoc ListenerFactoryLogic::createListener
Ogre::CompositorInstance::Listener* BloomLogic::createListener(Ogre::CompositorInstance* instance)
{
	mListener = new BloomListener;
	return mListener;
}

/// @brief Sets the BlurWeight parameter of the Bloom compositor.
/// @param n	The new blur weight.
void BloomLogic::setBlurWeight (float n)
{
	((BloomListener*) mListener)->blurWeight = n;
}

/// @brief Sets the OriginalImageWeight of the Bloom compositor.
/// @param n	The new original image weighting.
void BloomLogic::setOriginalWeight (float n)
{
	((BloomListener*) mListener)->originalWeight = n;
}


/*-------------------- RADIAL BLUR LOGIC --------------------*/
/// @copydoc ListenerFactoryLogic::createListener
Ogre::CompositorInstance::Listener* RadialBlurLogic::createListener(Ogre::CompositorInstance* instance)
{
	mListener = new RadialBlurListener;
	return mListener;
}

/// @brief Sets the sampleDist parameter of the Radial Blur compositor.
/// @param n	The new blur distance.
void RadialBlurLogic::setBlurDistance (float n)
{
	((RadialBlurListener*) mListener)->blurDistance = n;
}

/// @brief Sets the sampleStrength parameter of the Radial Blur compositor.
/// @param n	The new blur strength.
void RadialBlurLogic::setBlurStrength (float n)
{
	((RadialBlurListener*) mListener)->blurStrength = n;
}


/*-------------------- MOTION BLUR LOGIC --------------------*/
/// @copydoc ListenerFactoryLogic::createListener
Ogre::CompositorInstance::Listener* MotionBlurLogic::createListener(Ogre::CompositorInstance* instance)
{
	mListener = new MotionBlurListener;
	return mListener;
}

/// @brief Sets the blur parameter of the Motion Blur compositor.
/// @param n	The new blur strength.
void MotionBlurLogic::setBlurStrength (float n)
{
	((MotionBlurListener*) mListener)->blurStrength = n;
}



/*-------------------- COMPOSITOR LISTENERS --------------------*/
/*-------------------- BLOOM LISTENER --------------------*/
/// @brief Constructor.
BloomListener::BloomListener()
{
	blurWeight = 0.15f;
	originalWeight = 1.0f;
}

/// @brief Deconstructor.
BloomListener::~BloomListener()
{
}

/// @copydoc CompositorInstance::Listener::notifyMaterialSetup
void BloomListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

/// @copydoc CompositorInstance::Listener::notifyMaterialRender
void BloomListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if (pass_id == 700)
	{
		fpParams->setNamedConstant("BlurWeight", blurWeight);
		fpParams->setNamedConstant("OriginalImageWeight", originalWeight);
	}
}


/*-------------------- RADIAL BLUR LISTENER --------------------*/
/// @brief Constructor.
RadialBlurListener::RadialBlurListener()
{
	blurDistance = 0.2f;
	blurStrength = 1.0f;
}

/// @brief Deconstructor.
RadialBlurListener::~RadialBlurListener()
{
}

/// @copydoc CompositorInstance::Listener::notifyMaterialSetup
void RadialBlurListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

/// @copydoc CompositorInstance::Listener::notifyMaterialRender
void RadialBlurListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if (pass_id == 700)
	{
		fpParams->setNamedConstant("sampleDist", blurDistance);
		fpParams->setNamedConstant("sampleStrength", blurStrength);
	}
}


/*-------------------- MOTION BLUR LISTENER --------------------*/
/// @brief Constructor.
MotionBlurListener::MotionBlurListener()
{
	blurStrength = 1.8f;
}

/// @brief Deconstructor.
MotionBlurListener::~MotionBlurListener()
{
}

/// @copydoc CompositorInstance::Listener::notifyMaterialSetup
void MotionBlurListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

/// @copydoc CompositorInstance::Listener::notifyMaterialRender
void MotionBlurListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	OutputDebugString("yoohoo\n");
	if (pass_id == 700)
	{
		fpParams->setNamedConstant("blur", blurStrength);
	}
}
