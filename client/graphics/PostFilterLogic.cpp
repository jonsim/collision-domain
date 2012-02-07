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
/*-------------------- HDR LOGIC --------------------*/
/// @copydoc ListenerFactoryLogic::createListener
Ogre::CompositorInstance::Listener* HDRLogic::createListener(Ogre::CompositorInstance* instance)
{
	mListener = new HDRListener;
	return mListener;
}

/// @brief Sets the BlurWeight parameter of the Bloom compositor.
/// @param n	The new blur weight.
void HDRLogic::setBloomSize (float n)
{
	((HDRListener*) mListener)->bloomSize = n;
}


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
/*-------------------- HDR LISTENER --------------------*/
/// @brief Constructor.
HDRListener::HDRListener()
{
	//bloomSize = 0.15f;
}

/// @brief Deconstructor.
HDRListener::~HDRListener()
{
}

void HDRListener::notifyViewportSize (int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
}

void HDRListener::notifyCompositor (Ogre::CompositorInstance* instance)
{
	// Get some RTT dimensions for later calculations
	Ogre::CompositionTechnique::TextureDefinitionIterator defIter =	instance->getTechnique()->getTextureDefinitionIterator();
	while (defIter.hasMoreElements())
	{
		Ogre::CompositionTechnique::TextureDefinition* def =
			defIter.getNext();
		if(def->name == "rt_bloom0")
		{
			bloomSize = (int)def->width; // should be square
			// Calculate gaussian texture offsets & weights
			float deviation = 3.0f;
			float texelSize = 1.0f / (float) bloomSize;

			// central sample, no offset
			textureWeights[0][0]  = textureWeights[0][1] = textureWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
			textureWeights[0][3]  = 1.0f;
			textureHOffsets[0][0] = 0.0f;
			textureHOffsets[0][1] = 0.0f;
			textureVOffsets[0][0] = 0.0f;
			textureVOffsets[0][1] = 0.0f;

			// 'pre' samples
			for (int i = 1; i < 8; ++i)
			{
				textureWeights[i][0]  = textureWeights[i][1] = textureWeights[i][2] = 1.25f * Ogre::Math::gaussianDistribution(i, 0, deviation);
				textureWeights[i][3]  = 1.0f;
				textureHOffsets[i][0] = i * texelSize;
				textureHOffsets[i][1] = 0.0f;
				textureVOffsets[i][0] = 0.0f;
				textureVOffsets[i][1] = i * texelSize;
			}
			// 'post' samples
			for (int i = 8; i < 15; ++i)
			{
				textureWeights[i][0]  = textureWeights[i][1] = textureWeights[i][2] = textureWeights[i - 7][0];
				textureWeights[i][3]  = 1.0f;
				textureHOffsets[i][0] = -textureHOffsets[i - 7][0];
				textureHOffsets[i][1] = 0.0f;
				textureVOffsets[i][0] = 0.0f;
				textureVOffsets[i][1] = -textureVOffsets[i - 7][1];
			}

		}
	}
}

void HDRListener::notifyMaterialSetup (Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
	//case 994: // rt_lum4
	case 993: // rt_lum3
	case 992: // rt_lum2
	case 991: // rt_lum1
	case 990: // rt_lum0
		break;
	case 800: // rt_brightpass
		break;
	case 701: // rt_bloom1
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", textureHOffsets[0], 15);
			fparams->setNamedConstant("sampleWeights", textureWeights[0], 15);

			break;
		}
	case 700: // rt_bloom0
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", textureVOffsets[0], 15);
			fparams->setNamedConstant("sampleWeights", textureWeights[0], 15);

			break;
		}
	}
}


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
	if (pass_id == 700)
	{
		Ogre::GpuProgramParametersSharedPtr fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
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
	blurStrength = 0.8f;
}

/// @brief Deconstructor.
MotionBlurListener::~MotionBlurListener()
{
}

/// @copydoc CompositorInstance::Listener::notifyMaterialSetup
void MotionBlurListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if (pass_id == 700)
	{
		Ogre::GpuProgramParametersSharedPtr fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
		fpParams->setNamedConstant("blur", blurStrength);
	}
}
