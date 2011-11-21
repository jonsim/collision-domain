/*------------------------------------------------------------------------------
  File:     GraphicsApplication.cpp
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.cpp).
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "stdafx.h"
#include "GraphicsApplication.h"



/******************** METHOD DEFINITIONS ********************/

/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::GraphicsApplication(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Constructor.
 ------------------------------------------------------------------------------*/
GraphicsApplication::GraphicsApplication(void)
{
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::~GraphicsApplication(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Deconstructor.
 ------------------------------------------------------------------------------*/
GraphicsApplication::~GraphicsApplication(void)
{
}


void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}


void GraphicsApplication::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}


void GraphicsApplication::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}


void GraphicsApplication::configureTerrainDefaults(Ogre::Light* light)
{
    // Configure global
    mTerrainGlobals->setMaxPixelError(8);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(3000);

    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600; // due terrain.png is 8 bpp
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createCamera(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Creates and positions the scene's camera.
 ------------------------------------------------------------------------------
void GraphicsApplication::createCamera(void)
{
    // create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // set its position, direction  
    mCamera->setPosition(Ogre::Vector3(0, 50, 200));
    mCamera->lookAt(Ogre::Vector3(0, 0, 0));

    // set the near and far clip distances (to infinite if the render system supports it)
    mCamera->setNearClipDistance(5);
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
        mCamera->setFarClipDistance(0);
    else
        mCamera->setFarClipDistance(50000);

    // create a default camera controller
    mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}*/


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createViewports(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Creates and scales the scene's viewport(s).
 ------------------------------------------------------------------------------
void GraphicsApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);

    // set the background to black
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}*/


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createScene(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Loads and adds all entities to the scene.
 ------------------------------------------------------------------------------*/
void GraphicsApplication::createScene(void)
{
    // create entities
	//Ogre::Entity* ogreHead1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
    //Ogre::Entity* ogreHead2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
    //ogreHead1->setCastShadows(true);
    //ogreHead2->setCastShadows(true);
    
    // create scene nodes for the entities
	//Ogre::SceneNode* headNode1  = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode1", Ogre::Vector3(-50, 25, 0));
    //Ogre::SceneNode* headNode2  = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode2", Ogre::Vector3(50, 25, 0));

    // attach the entities
	//headNode1->attachObject(ogreHead1);
    //headNode2->attachObject(ogreHead2);
    
    // Set the shadow renderer
    //mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Setup the camera
    mCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
    mCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
    mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(50000);
 
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }
 
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);


	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
     
	// Create the lights
    Ogre::Vector3 directionalLightDir(0.55, -0.3, 0.75);
    directionalLightDir.normalise();
    Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight1");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour( Ogre::ColourValue::White);
    directionalLight->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    directionalLight->setDirection(directionalLightDir);

    // Create the skybox
    mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);


    // Create the terrain.
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

    configureTerrainDefaults(directionalLight);

    for (long x = 0; x < 1; ++x)
        for (long y = 0; y < 1; ++y)
            defineTerrain(x, y);
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);

    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }

    mTerrainGroup->freeTemporaryResources();
}

void GraphicsApplication::createFrameListener(void)
{
    GraphicsCore::createFrameListener();
 
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350); 
}

void GraphicsApplication::destroyScene(void)
{
    OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}

bool GraphicsApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = GraphicsCore::frameRenderingQueued(evt);
 
    if (mTerrainGroup->isDerivedDataUpdateInProgress())
    {
        mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
        mInfoLabel->show();
        if (mTerrainsImported)
        {
            mInfoLabel->setCaption("Building terrain, please wait...");
        }
        else
        {
            mInfoLabel->setCaption("Updating textures, patience...");
        }
    }
    else
    {
        mTrayMgr->removeWidgetFromTray(mInfoLabel);
        mInfoLabel->hide();
        if (mTerrainsImported)
        {
            mTerrainGroup->saveAllTerrains(true);
            mTerrainsImported = false;
        }
    }
 
    return ret;
}


/*------------------------------------------------------------------------------
  Method:       INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
  Parameters:   Unknown.
  Outputs:      Unknown.
  Purpose:      Does something for Win32. Unknown.
 ------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        GraphicsApplication app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
