
/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


// Listener Factory Logic shit
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



// Specific logic implementations
/// @copydoc ListenerFactoryLogic::createListener
Ogre::CompositorInstance::Listener* GaussianBlurLogic::createListener(Ogre::CompositorInstance* instance)
{
	GaussianListener* listener = new GaussianListener;
	Ogre::Viewport* vp = instance->getChain()->getViewport();
	listener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
	return listener;
}

// Specific logic implementations
/// @copydoc ListenerFactoryLogic::createListener
Ogre::CompositorInstance::Listener* BloomLogic::createListener(Ogre::CompositorInstance* instance)
{
	OutputDebugString("Created new BloomLogic\n");
	mListener = new BloomListener;
	return mListener;
	
	/*BloomListener* listener = new BloomListener;
	Ogre::Viewport* vp = instance->getChain()->getViewport();
	listener->notifyViewportSize(vp->getActualWidth(), vp->getActualHeight());
	return listener;*/
}


/*************************************************************************
BloomListner Methods
*************************************************************************/
//---------------------------------------------------------------------------
BloomListener::BloomListener()
{
	blurWeight = 1.0f;
	originalWeight = 1.0f;
	OutputDebugString("Created new BloomListener\n");
}
//---------------------------------------------------------------------------
BloomListener::~BloomListener()
{
}
//---------------------------------------------------------------------------
void BloomListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

void BloomListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if (pass_id == 700)
	{
		fpParams->setNamedConstant("BlurWeight", blurWeight);
		fpParams->setNamedConstant("OriginalImageWeight", originalWeight);
	}
}



/*************************************************************************
GaussianListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
GaussianListener::GaussianListener()
{

}
//---------------------------------------------------------------------------
GaussianListener::~GaussianListener()
{
}
//---------------------------------------------------------------------------
void GaussianListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
	// Calculate gaussian texture offsets & weights
	float deviation = 3.0f;
	float texelSize = 1.0f / (float)std::min(mVpWidth, mVpHeight);

	// central sample, no offset
	mBloomTexWeights[0][0] = mBloomTexWeights[0][1] = mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
	mBloomTexWeights[0][3] = 1.0f;
	mBloomTexOffsetsHorz[0][0] = 0.0f;
	mBloomTexOffsetsHorz[0][1] = 0.0f;
	mBloomTexOffsetsVert[0][0] = 0.0f;
	mBloomTexOffsetsVert[0][1] = 0.0f;

	// 'pre' samples
	for(int i = 1; i < 8; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = mBloomTexWeights[i][2] = Ogre::Math::gaussianDistribution(i, 0, deviation);
		mBloomTexWeights[i][3] = 1.0f;
		mBloomTexOffsetsHorz[i][0] = i * texelSize;
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = i * texelSize;
	}
	// 'post' samples
	for(int i = 8; i < 15; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
		mBloomTexWeights[i][3] = 1.0f;
		mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
	case 701: // blur horz
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	case 700: // blur vert
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	}
}

void GaussianListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}

