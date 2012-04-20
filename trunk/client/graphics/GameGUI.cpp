#include "stdafx.h"
#include "GameGUI.h"
#include "boost/algorithm/string.hpp"




/*-------------------- SPAWN SCREEN --------------------*/
void GameGUI::setupSpawnScreen (CEGUI::Window* guiWindow)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

        // Load the layout file for connect box
        CEGUI::Window *pLayout = winMgr.loadWindowLayout("SpawnScreen.layout");

        // Add to gui overlay window
        guiWindow->addChildWindow( pLayout );

        // Setup the image
    spawnScreenImageSet = &CEGUI::ImagesetManager::getSingleton().create("SpawnScreen.imageset");
        spawnScreenImage = winMgr.getWindow("/SpawnScreen/Vehicle/imgVehicle");
    spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("CoupeBlue")));

    // Get handles to the buttons
    // Page 1 buttons
        CEGUI::Window* p1btnBlueTeam   = winMgr.getWindow("/SpawnScreen/Team/btnBlue");
        CEGUI::Window* p1btnRedTeam    = winMgr.getWindow("/SpawnScreen/Team/btnRed");
        CEGUI::Window* p1btnAutoAssign = winMgr.getWindow("/SpawnScreen/Team/btnAuto");
        CEGUI::Window* p1btnSpectator  = winMgr.getWindow("/SpawnScreen/Team/btnSpectate");
        CEGUI::Window* p1btnProjector  = winMgr.getWindow("/SpawnScreen/Team/btnProjector");
        p1btnBlueTeam->subscribeEvent(  CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p1btnBlueTeam,   this));
        p1btnRedTeam->subscribeEvent(   CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p1btnRedTeam,    this));
        p1btnAutoAssign->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p1btnAutoAssign, this));
        p1btnSpectator->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p1btnSpectator,  this));
        p1btnProjector->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p1btnProjector,  this));

    // Page 2 buttons
        CEGUI::Window* p2btnCoupe     = winMgr.getWindow("/SpawnScreen/Vehicle/btnCoupe");
        CEGUI::Window* p2btnHatchback = winMgr.getWindow("/SpawnScreen/Vehicle/btnHatchback");
        CEGUI::Window* p2btnTruck     = winMgr.getWindow("/SpawnScreen/Vehicle/btnTruck");
        CEGUI::Window* p2btnCancel    = winMgr.getWindow("/SpawnScreen/Vehicle/btnCancel");
        CEGUI::Window* p2btnConfirm   = winMgr.getWindow("/SpawnScreen/Vehicle/btnConfirm");
        p2btnCoupe->subscribeEvent(    CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p2btnCoupe,     this));
        p2btnHatchback->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p2btnHatchback, this));
        p2btnTruck->subscribeEvent(    CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p2btnTruck,     this));
        p2btnCancel->subscribeEvent(   CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p2btnCancel,    this));
        p2btnConfirm->subscribeEvent(  CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameGUI::SpawnScreen_p2btnConfirm,   this));

        CEGUI::MouseCursor::getSingleton().show();

        spawnScreenCarSelection = CAR_BANGER;
}

void GameGUI::showSpawnScreenPage1 (void)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

    // Get references to the pages.
    CEGUI::Window* mainWindow = winMgr.getWindow("/SpawnScreen");
    CEGUI::Window* page1 = winMgr.getWindow("/SpawnScreen/Team");
    CEGUI::Window* page2 = winMgr.getWindow("/SpawnScreen/Vehicle");

    // Show the correct pages
    mainWindow->setVisible(true);
    page2->setVisible(false);
    page1->setVisible(true);
}

void GameGUI::showSpawnScreenPage2 (void)
{
    CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

    // Get references to the pages.
    CEGUI::Window* mainWindow     = winMgr.getWindow("/SpawnScreen");
    CEGUI::Window* page1          = winMgr.getWindow("/SpawnScreen/Team");
    CEGUI::Window* page2          = winMgr.getWindow("/SpawnScreen/Vehicle");
        CEGUI::Window* p2btnCoupe     = winMgr.getWindow("/SpawnScreen/Vehicle/btnCoupe");
        CEGUI::Window* p2btnHatchback = winMgr.getWindow("/SpawnScreen/Vehicle/btnHatchback");
        CEGUI::Window* p2btnTruck     = winMgr.getWindow("/SpawnScreen/Vehicle/btnTruck");

    // Update the page's image
    if (spawnScreenTeamSelection == 1)
    {
        page2->setProperty("Text", "Blue Team: Select a Vehicle");
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("CoupeBlue")));
    }
    else if (spawnScreenTeamSelection == 2)
    {
        page2->setProperty("Text", "Red Team: Select a Vehicle");
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("CoupeRed")));
    }

    // Update the page's buttons
    p2btnCoupe->setProperty(    "Disabled", "true");
    p2btnHatchback->setProperty("Disabled", "false");
    p2btnTruck->setProperty(    "Disabled", "false");

    // Show the correct pages
    mainWindow->setVisible(true);
    page1->setVisible(false);
    page2->setVisible(true);
}

void GameGUI::showSpawnScreenErrorText (const char* errorText)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

    CEGUI::Window* txtError = winMgr.getWindow("/SpawnScreen/txtError");
    txtError->setProperty("Text", errorText);
    txtError->show();
}

void GameGUI::hideSpawnScreenErrorText (void)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

    CEGUI::Window* txtError = winMgr.getWindow("/SpawnScreen/txtError");
    txtError->hide();
}

void GameGUI::closeSpawnScreen (void)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

    CEGUI::Window* mainWindow = winMgr.getWindow("/SpawnScreen");
    mainWindow->hide();

        CEGUI::MouseCursor::getSingleton().hide();
}

bool GameGUI::SpawnScreen_p1btnBlueTeam (const CEGUI::EventArgs& args)
{
    hideSpawnScreenErrorText();

    spawnScreenTeamSelection = 1;
    GameCore::mNetworkCore->sendTeamSelect( BLUE_TEAM );

    return true;
}

bool GameGUI::SpawnScreen_p1btnRedTeam (const CEGUI::EventArgs& args)
{
    hideSpawnScreenErrorText();

    spawnScreenTeamSelection = 2;
    GameCore::mNetworkCore->sendTeamSelect( RED_TEAM );

    return true;
}

bool GameGUI::SpawnScreen_p1btnAutoAssign (const CEGUI::EventArgs& args)
{
    hideSpawnScreenErrorText();

    spawnScreenTeamSelection = 1;
    GameCore::mNetworkCore->sendTeamSelect( NO_TEAM );

    return true;
}

bool GameGUI::SpawnScreen_p1btnSpectator (const CEGUI::EventArgs& args)
{
    closeSpawnScreen();
    GameCore::mPlayerPool->getLocalPlayer()->setPlayerState( PLAYER_STATE_SPECTATE );

    return true;
}

bool GameGUI::SpawnScreen_p1btnProjector (const CEGUI::EventArgs& args)
{
	closeSpawnScreen();
    GameCore::mClientGraphics->setupProjector();

    return true;
}

bool GameGUI::SpawnScreen_p2btnCoupe (const CEGUI::EventArgs& args)
{
    CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* p2btnCoupe     = winMgr.getWindow("/SpawnScreen/Vehicle/btnCoupe");
    CEGUI::Window* p2btnHatchback = winMgr.getWindow("/SpawnScreen/Vehicle/btnHatchback");
    CEGUI::Window* p2btnTruck     = winMgr.getWindow("/SpawnScreen/Vehicle/btnTruck");
    p2btnCoupe->setProperty(    "Disabled", "true");
    p2btnHatchback->setProperty("Disabled", "false");
    p2btnTruck->setProperty(    "Disabled", "false");

    if (spawnScreenTeamSelection == 1)
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("CoupeBlue")));
    else if (spawnScreenTeamSelection == 2)
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("CoupeRed")));

    spawnScreenCarSelection = CAR_BANGER;

    return true;
}

bool GameGUI::SpawnScreen_p2btnHatchback (const CEGUI::EventArgs& args)
{
    CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* p2btnCoupe     = winMgr.getWindow("/SpawnScreen/Vehicle/btnCoupe");
    CEGUI::Window* p2btnHatchback = winMgr.getWindow("/SpawnScreen/Vehicle/btnHatchback");
    CEGUI::Window* p2btnTruck     = winMgr.getWindow("/SpawnScreen/Vehicle/btnTruck");
    p2btnCoupe->setProperty(    "Disabled", "false");
    p2btnHatchback->setProperty("Disabled", "true");
    p2btnTruck->setProperty(    "Disabled", "false");

    if (spawnScreenTeamSelection == 1)
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("HatchbackBlue")));
    else if (spawnScreenTeamSelection == 2)
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("HatchbackRed")));

    spawnScreenCarSelection = CAR_SMALL;

    return true;
}

bool GameGUI::SpawnScreen_p2btnTruck (const CEGUI::EventArgs& args)
{
    CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* p2btnCoupe     = winMgr.getWindow("/SpawnScreen/Vehicle/btnCoupe");
    CEGUI::Window* p2btnHatchback = winMgr.getWindow("/SpawnScreen/Vehicle/btnHatchback");
    CEGUI::Window* p2btnTruck     = winMgr.getWindow("/SpawnScreen/Vehicle/btnTruck");
    p2btnCoupe->setProperty(    "Disabled", "false");
    p2btnHatchback->setProperty("Disabled", "false");
    p2btnTruck->setProperty(    "Disabled", "true");

    if (spawnScreenTeamSelection == 1)
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("TruckBlue")));
    else if (spawnScreenTeamSelection == 2)
        spawnScreenImage->setProperty("Image", CEGUI::PropertyHelper::imageToString(&spawnScreenImageSet->getImage("TruckRed")));

    spawnScreenCarSelection = CAR_TRUCK;

    return true;
}

bool GameGUI::SpawnScreen_p2btnConfirm (const CEGUI::EventArgs& args)
{
    closeSpawnScreen();
    GameCore::mNetworkCore->sendSpawnRequest( spawnScreenCarSelection );

    return true;
}

bool GameGUI::SpawnScreen_p2btnCancel (const CEGUI::EventArgs& args)
{
    showSpawnScreenPage1();

    return true;
}




/*-------------------- DEV CONSOLE --------------------*/
void GameGUI::setupConsole (CEGUI::Window* guiWindow)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

        // Load the layout file for connect box
        CEGUI::Window *pLayout = winMgr.loadWindowLayout( "Console.layout" );

        // Add to gui overlay window
        guiWindow->addChildWindow( pLayout );

        // Get handles to some of the objects
        CEGUI::Window *consoleFrame = winMgr.getWindow( "/Console" );
        CEGUI::Window *inputText = winMgr.getWindow( "/Console/input" );
        CEGUI::Window *cmdInput = winMgr.getWindow( "/Console/cmdInput" );

        consoleFrame->subscribeEvent( CEGUI::FrameWindow::EventDeactivated,
                CEGUI::Event::Subscriber( &GameGUI::Console_Off, this ) );
        inputText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, 
                CEGUI::Event::Subscriber( &GameGUI::Console_Send, this ) );
        cmdInput->subscribeEvent( CEGUI::PushButton::EventClicked,  
                CEGUI::Event::Subscriber( &GameGUI::Console_Send, this ) );

        CEGUI::MouseCursor::getSingleton().show();

        // Don't actually show the dev console yet
        consoleFrame->hide();
}

void GameGUI::toggleConsole (void)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
        // Get handles to some of the objects
        CEGUI::Window *consoleFrame = winMgr.getWindow( "/Console" );
        if( consoleFrame->isVisible() )
                consoleFrame->hide();
        else
        {
                // Show the console frame and give the inputbox focus
                consoleFrame->show();
                winMgr.getWindow( "/Console/input" )->activate();
        }
}

bool GameGUI::Console_Send (const CEGUI::EventArgs &args)
{
        CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();

        CEGUI::Editbox *inputText = 
                static_cast<CEGUI::Editbox*> ( mWinMgr.getWindow( "/Console/input" ) );

        char *szInput = (char*)inputText->getText().c_str();

        CEGUI::MultiLineEditbox *consoleBuffer = 
                static_cast<CEGUI::MultiLineEditbox*> ( mWinMgr.getWindow( "/Console/buffer" ) );

        consoleBuffer->appendText( inputText->getText() );

    std::string strInput = inputText->getText().c_str();

    if( strInput.length() >= 1 )
    {
        std::vector<std::string> strTokens;
        boost::split( strTokens, strInput, boost::is_any_of( "\t " ) );
        consoleBuffer->appendText( strTokens.at(0) );
        if( strTokens.size() >= 1)
        {
            if( strTokens.at(0) == "exit" )
                mWinMgr.getWindow( "/Console" )->hide();
            else if (!strnicmp(szInput, "weather ", 8))
                GameCore::mClientGraphics->setWeather(atoi(&szInput[8]) - 1);
            else if (!stricmp(szInput, "benchmark"))
                GameCore::mClientGraphics->startBenchmark(0);
            else if (!stricmp(szInput, "ass"))
                GameCore::mClientGraphics->generateShrapnel(Ogre::Vector3(0, -9, 0), BLUE_TEAM);
            else if (!strnicmp(szInput, "b1 ", 3))
                GameCore::mClientGraphics->loadBloom(GameCore::mClientGraphics->mCamera->getViewport(), 1, atof(&szInput[3]), -1.0f);
            else if (!strnicmp(szInput, "b2 ", 3))
                GameCore::mClientGraphics->loadBloom(GameCore::mClientGraphics->mCamera->getViewport(), 1, -1.0f, atof(&szInput[3]));
            else if (!strnicmp(szInput, "mb ", 3))
                GameCore::mClientGraphics->loadMotionBlur(GameCore::mClientGraphics->mCamera->getViewport(), 1, atof(&szInput[3]));
            else if (!stricmp(szInput, "wireframe on"))
                GameCore::mClientGraphics->mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
            else if (!stricmp(szInput, "wireframe off"))
                GameCore::mClientGraphics->mCamera->setPolygonMode(Ogre::PM_SOLID);
            else if (!stricmp(szInput, "bitsoff"))
                GameCore::mPlayerPool->getLocalPlayer()->getCar()->makeBitsFallOff();
            else if( strTokens.at(0) == "movdbg" )
            {
                if( strTokens.size() >= 4 )
                {
                    try
                    {
                        float fX = boost::lexical_cast<float>(strTokens.at(1));
                        float fY = boost::lexical_cast<float>(strTokens.at(2));
                        float fZ = boost::lexical_cast<float>(strTokens.at(3));
                        Ogre::Vector3 trans( fX, fY, fZ );
                        char szPrint[128];
                        sprintf( szPrint, "Moved debug shape: [x] %f [y] %f [z] %f", fX, fY, fZ );
                        consoleBuffer->appendText( szPrint );
                    }
                    catch( boost::bad_lexical_cast &) {}
                }
            }
            else if( strTokens.at(0) == "setcam" )
            {
                if( strTokens.size() == 2 )
                {
                    try
                    {
                        CAM_TYPE t = (CAM_TYPE) boost::lexical_cast<int>(strTokens.at(1));
                        GameCore::mClientGraphics->mGameCam->setCamType(t);
                    }
                    catch( boost::bad_lexical_cast &) {}
                }
            }
        }
    }

    inputText->setText( "" );

        //if( !stricmp( szInput, "exit" ) )
        //      mWinMgr.getWindow( "/Console" )->hide();

        return true;
}

bool GameGUI::Console_Off (const CEGUI::EventArgs &args)
{
        CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();
        mWinMgr.getWindow( "/Console" )->hide();
        return true;
}

/*-------------------- DEV Chatbox --------------------*/
void GameGUI::setupChatbox (CEGUI::Window* guiWindow)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

        // Load the layout file for connect box
        CEGUI::Window *pLayout = winMgr.loadWindowLayout( "Chatbox.layout" );

        // Add to gui overlay window
        guiWindow->addChildWindow( pLayout );

        // Get handles to some of the objects
        CEGUI::Window *chatboxFrame = winMgr.getWindow( "/Chatbox" );
        CEGUI::Window *inputText = winMgr.getWindow( "/Chatbox/input" );

        inputText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, 
                CEGUI::Event::Subscriber( &GameGUI::Chatbox_Send, this ) );

        CEGUI::MouseCursor::getSingleton().show();
        inputText->hide();
}

void GameGUI::toggleChatbox (void)
{
        CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
        // Get handles to some of the objects
        CEGUI::Window *chatboxFrame = winMgr.getWindow( "/Chatbox/input" );
        if( chatboxFrame->isVisible() )
                chatboxFrame->hide();
        else
        {
                // Show the Chatbox frame and give the inputbox focus
                chatboxFrame->show();
                winMgr.getWindow( "/Chatbox/input" )->activate();
        }
}

bool GameGUI::Chatbox_Send (const CEGUI::EventArgs &args)
{
        CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();

        CEGUI::Editbox *inputText = 
                static_cast<CEGUI::Editbox*> ( mWinMgr.getWindow( "/Chatbox/input" ) );

        char *szInput = (char*)inputText->getText().c_str();
    GameCore::mNetworkCore->sendChatMessage( szInput );

        inputText->setText( "" );

        mWinMgr.getWindow( "/Chatbox/input" )->hide();

        return true;
}

void GameGUI::chatboxAddMessage (const char *szNickname, char *szMessage)
{
    char szBuffer[256];
    if (!strcmp(szNickname, "Admin"))
        sprintf( szBuffer, "[colour='FF6DDD77']Admin:[colour='FFFFFFFF'] %s", szMessage );
    else
        sprintf( szBuffer, "[colour='FFED9DAA']%s:[colour='FFFFFFFF'] %s", szNickname, szMessage );

    CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();

    CEGUI::Listbox *lstHistory = 
        static_cast<CEGUI::Listbox*> ( mWinMgr.getWindow( "/Chatbox/buffer" ) );

        CEGUI::ListboxTextItem *newItem;
        if( lstHistory->getItemCount() == 6 )
        {
                newItem = static_cast<CEGUI::ListboxTextItem*> 
                        ( lstHistory->getListboxItemFromIndex( 0 ) );
                newItem->setAutoDeleted( false );
                lstHistory->removeItem( newItem );
                newItem->setAutoDeleted( true );
                newItem->setText( szBuffer );
        }
        else
        {
                newItem = new CEGUI::ListboxTextItem( szBuffer );
        }
        
        lstHistory->addItem( newItem );
        lstHistory->ensureItemIsVisible( lstHistory->getItemCount() );
}




/* In-game HUD */
/* FPS Counter */
void GameGUI::setupFPSCounter (CEGUI::Window* guiWindow)
{
    // Setup the FPS Counter
        CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().
        createWindow( "Vanilla/StaticText", "root_wnd/fps" );
    fps->setText( "fps: " );
        fps->setSize( CEGUI::UVector2(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
        CEGUI::System::getSingleton().setGUISheet( guiWindow );
        guiWindow->addChildWindow( fps );
}

/*-------------------- SPEEDOMETER --------------------*/
/// @brief Draws the speedo on-screen
void GameGUI::setupSpeedo (void)
{
        // Create our speedometer overlays
        Ogre::Overlay *olSpeedo = Ogre::OverlayManager::getSingleton().create( "OVERLAY_SPD" );
        olSpeedo->setZOrder( 500 );
        olSpeedo->show();

        olcSpeedo = static_cast<Ogre::OverlayContainer*> ( Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "SPEEDO" ) );
        olcSpeedo->setMetricsMode( Ogre::GMM_PIXELS );
        olcSpeedo->setHorizontalAlignment( Ogre::GHA_LEFT );
        olcSpeedo->setVerticalAlignment( Ogre::GVA_BOTTOM );
        olcSpeedo->setDimensions( 250, 250 );
        olcSpeedo->setMaterialName( "speedo_main" );
        olcSpeedo->setPosition( 20, -270 );

        olSpeedo->add2D( olcSpeedo );

        oleNeedle = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "SPEEDONEEDLE" );
        oleNeedle->setMetricsMode( Ogre::GMM_PIXELS );
        oleNeedle->setDimensions( 250, 250 );
        oleNeedle->setMaterialName( "speedo_needle" );
    
        olcSpeedo->addChild( oleNeedle );
    
    /*Ogre::OverlayElement *hub = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "SPEEDOHUB" );
        hub->setMetricsMode( Ogre::GMM_PIXELS );
        hub->setDimensions( 250, 250 );
        hub->setMaterialName( "speedo_hub" );

        olcSpeedo->addChild( hub );*/

    // The needle/hub layering was incorrect before, adding another container implicitly puts it on top
    
        Ogre::OverlayContainer *olcHub = static_cast<Ogre::OverlayContainer*> ( Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "SPEEDO_HUB" ) );
        olcHub->setMetricsMode( Ogre::GMM_PIXELS );
        olcHub->setHorizontalAlignment( Ogre::GHA_LEFT );
        olcHub->setVerticalAlignment( Ogre::GVA_BOTTOM );
        olcHub->setDimensions( 250, 250 );
        olcHub->setMaterialName( "speedo_hub" );
        olcHub->setPosition( 20, -270 );
    
        olSpeedo->add2D( olcHub );
}

void GameGUI::updateSpeedo (void)
{
    updateSpeedo(GameCore::mPlayerPool->getLocalPlayer()->getCar()->getCarMph(),
                 GameCore::mPlayerPool->getLocalPlayer()->getCar()->getGear());
}

/// @brief      Update the rotation of the speedo needle
/// @param      fSpeed  Float containing speed of car in mph
/// @param  iGear   Current car gear
void GameGUI::updateSpeedo (float fSpeed, int iGear)
{
        if( fSpeed < 0 )
                fSpeed *= -1;

        if( fSpeed > 220 )
                fSpeed = 220;

        float iDegree = 58; // This is 0 for some reason

        // 1 mph = 298 / 220 degrees
    
        iDegree = 58 - ( fSpeed * ( 298.0f / 220.0f ) );
    
        Ogre::Material *matNeedle = oleNeedle->getMaterial().get();
        Ogre::TextureUnitState *texNeedle = matNeedle->getTechnique( 0 )->getPass( 0 )->getTextureUnitState( 0 );
        texNeedle->setTextureRotate( Ogre::Degree( iDegree ) );

    if( iGear >= 0 )
    {
        char cnum[2];
        itoa( iGear, cnum, 10 );
        cnum[1] = '\0';
        
        char matname[32];
        sprintf( matname, "gear%s", cnum );
        oleGear->setMaterialName( matname );
    }
    else
        oleGear->setMaterialName( "gearoff" );
}

/*-------------------- GEAR DISPLAY --------------------*/
/// @brief Draws the gear display
void GameGUI::setupGearDisplay (void)
{
        oleGear = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "GEAR" );

        oleGear->setMetricsMode( Ogre::GMM_PIXELS );
        oleGear->setHorizontalAlignment( Ogre::GHA_LEFT );
        oleGear->setVerticalAlignment( Ogre::GVA_BOTTOM );
        oleGear->setDimensions( 32, 57 );
        oleGear->setMaterialName( "gearoff" );
        oleGear->setPosition( 109, -73 );

        olcSpeedo->addChild( oleGear );

        updateSpeedo( 0, -1 );
}

void GameGUI::updateCounters (void)
{
        static char szFPS[64];

        CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/fps" );
        sprintf( szFPS,   "FPS: %.2f", GameCore::mClientGraphics->mWindow->getAverageFPS());
        fps->setText( szFPS );
}

void GameGUI::setupDamageDisplay (void)
{
    int width = 82, height = 169;

    // setup body image
    {
            oleDamage = static_cast<Ogre::OverlayContainer*> ( Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "DAMAGE" ) );
            oleDamage->setMetricsMode( Ogre::GMM_PIXELS );
        
        /*{ // Bottom Left Aligned
            oleDamage->setHorizontalAlignment( Ogre::GHA_LEFT );
                oleDamage->setVerticalAlignment( Ogre::GVA_BOTTOM );
                oleDamage->setDimensions( width, height );
                oleDamage->setPosition( 20, -height - 290 );
        }*/

        { // Bottom Right Aligned
            oleDamage->setHorizontalAlignment( Ogre::GHA_RIGHT );
                oleDamage->setVerticalAlignment( Ogre::GVA_BOTTOM );
                oleDamage->setDimensions( width, height );
                oleDamage->setPosition( -width - 20, -height - 20 );
        }

            Ogre::Overlay *damage = Ogre::OverlayManager::getSingleton().create( "OVERLAY_DAMAGE" );
            damage->setZOrder( 500 );
            damage->show();
        damage->setScale(1.0,1.0);
        damage->add2D( oleDamage );
    }

    // setup individual parts
    oleDamageEngine = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "DAMAGE_ENGINE" );
    oleDamageFL     = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "DAMAGE_FL" );
    oleDamageFR     = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "DAMAGE_FR" );
    oleDamageRL     = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "DAMAGE_RL" );
    oleDamageRR     = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "DAMAGE_RR" );

        oleDamageEngine->setMetricsMode( Ogre::GMM_PIXELS );
    oleDamageFL    ->setMetricsMode( Ogre::GMM_PIXELS );
    oleDamageFR    ->setMetricsMode( Ogre::GMM_PIXELS );
    oleDamageRL    ->setMetricsMode( Ogre::GMM_PIXELS );
    oleDamageRR    ->setMetricsMode( Ogre::GMM_PIXELS );
    
        /*oleDamageEngine->setDimensions( width, height );
    oleDamageFL    ->setDimensions( width, height );
    oleDamageFR    ->setDimensions( width, height );
    oleDamageRL    ->setDimensions( width, height );
    oleDamageRR    ->setDimensions( width, height );*/
    
    // this is necessary as the images didn't line up pixel perfect when
    // wheel parts were the same size as the whole body (with transparency)
    // (the ogre overlays appear to do some unexpected scaling / processing)
    oleDamageEngine->setDimensions( 32, 43 );
    oleDamageFL    ->setDimensions( 24, 49 );
    oleDamageFR    ->setDimensions( 24, 49 );
    oleDamageRL    ->setDimensions( 24, 49 );
    oleDamageRR    ->setDimensions( 24, 49 );
    oleDamageEngine->setPosition( 25, 9 );
    oleDamageFL    ->setPosition( 0, 13 );
    oleDamageFR    ->setPosition( 58, 13 );
    oleDamageRL    ->setPosition( 0, 114 );
    oleDamageRR    ->setPosition( 58, 114 );

        oleDamage->addChild( oleDamageEngine );
    oleDamage->addChild( oleDamageFL );
    oleDamage->addChild( oleDamageFR );
    oleDamage->addChild( oleDamageRL );
    oleDamage->addChild( oleDamageRR );
    
    updateDamage(0, 0);
    updateDamage(1, 0);
    updateDamage(2, 0);
    updateDamage(3, 0);
    updateDamage(4, 0);
    updateDamage(5, 0);
}

// part 0-body, 1-engine, 2-fl, 3-fr, 4-rl, 5-rr. colour 0-green, 1-yellow, 2-red
void GameGUI::updateDamage (int part, int colour)
{
    std::string s = "damage_";

    switch (colour)
    {
        case 0:
            s += "g"; break;
        case 1:
            s += "y"; break;
        case 2:
            s += "r"; break;
        default:
            return;
    }
    
    switch (part)
    {
        case 0:
            oleDamage->setMaterialName( s + "_body" );
            break;
        case 1:
            oleDamageEngine->setMaterialName( s + "_engine" );
            break;
        case 2:
            oleDamageFL->setMaterialName( s + "_fl" );
            break;
        case 3:
            oleDamageFR->setMaterialName( s + "_fr" );
            break;
        case 4:
            oleDamageRL->setMaterialName( s + "_rl" );
            break;
        case 5:
            oleDamageRR->setMaterialName( s + "_rr" );
            break;
        default:
            return;
    }
}

void GameGUI::setupOverlays (CEGUI::Window* guiWindow)
{
    setupSpeedo();
    setupGearDisplay();
    updateSpeedo(0.0f, 0);
    
    setupDamageDisplay();
    //updateDamage();
}
