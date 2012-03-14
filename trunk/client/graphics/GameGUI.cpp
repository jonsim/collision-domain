#include "stdafx.h"
#include "GameGUI.h"
#include "GraphicsCore.h"
#include "boost/algorithm/string.hpp"

GameGUI::GameGUI()
{
}

GameGUI::~GameGUI()
{
}

void GameGUI::initialiseGUI()
{
	// Create the default font
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Font *pFont = &CEGUI::FontManager::getSingleton().create("Verdana-outline-10.font");
    CEGUI::System::getSingleton().setDefaultFont( pFont );

    CEGUI::Font *pOtherFont = &CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
    pOtherFont->setProperty( "PointSize", "6" );

	// Create skin scheme outlining widget (window) parameters
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

	// Register skin's default image set and cursor icon
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");

	// Tell CEGUI where to look for layouts
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	// Create an empty default window layer
	mSheet = CEGUI::WindowManager::getSingleton().
		createWindow( "DefaultWindow", "root_wnd" );

	CEGUI::Window *mph = CEGUI::WindowManager::getSingleton().
		createWindow( "Vanilla/StaticText", "root_wnd/mph" );

	mph->setText( "MPH: " );
	mph->setSize( CEGUI::UVector2(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
    mph->setVisible( false );

	CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().
	createWindow( "Vanilla/StaticText", "root_wnd/fps" );
	fps->setText( "fps: " );
	fps->setSize( CEGUI::UVector2(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
	CEGUI::System::getSingleton().setGUISheet( mSheet );
	mSheet->addChildWindow( fps );

	mSheet->addChildWindow( mph );
}

/*-------------------- CONNECTION BOX --------------------*/

void GameGUI::setupConnectBox()
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	// Load the layout file for connect box
	CEGUI::Window *pLayout = winMgr.loadWindowLayout( "Connect.layout" );

	// Add to gui overlay window
	mSheet->addChildWindow( pLayout );

	// Get handles to some of the objects
	CEGUI::Window *hostText = winMgr.getWindow( "/Connect/host" );
    CEGUI::Window *nickText = winMgr.getWindow( "/Connect/nick" );
	CEGUI::Window *cmdCon = winMgr.getWindow( "/Connect/cmdCon" );
	CEGUI::Window *cmdQuit = winMgr.getWindow( "/Connect/cmdQuit" );

	// Give the host textbox focus
	hostText->activate();

	// Register callback for <Enter> press
	hostText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, 
		CEGUI::Event::Subscriber( &GameGUI::Connect_Host, this ) );
    nickText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, 
		CEGUI::Event::Subscriber( &GameGUI::Connect_Host, this ) );
	cmdCon->subscribeEvent( CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber( &GameGUI::Connect_Host, this ) );
	cmdQuit->subscribeEvent( CEGUI::PushButton::EventClicked,  
		CEGUI::Event::Subscriber( &GameGUI::Connect_Quit, this ) );
}

void GameGUI::closeConnectBox()
{
    mSheet->removeChildWindow( "/Connect" );
	CEGUI::MouseCursor::getSingleton().hide();
}

/// @brief  Check user has filled in all details, and connect, otherwise tab into the required field
bool GameGUI::Connect_Host( const CEGUI::EventArgs &args )
{
	CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Editbox *connectText = 
		static_cast<CEGUI::Editbox*> ( mWinMgr.getWindow( "/Connect/host" ) );
    CEGUI::Editbox *nickText = 
        static_cast<CEGUI::Editbox*> ( mWinMgr.getWindow( "/Connect/nick" ) );

    if( connectText->getText().empty() )
    {
        connectText->activate();
        return true;
    }

    if( nickText->getText().empty() )
    {
        nickText->activate();
        return true;
    }

	bool bResult = GameCore::mNetworkCore->Connect( 
		connectText->getText().c_str(), SERVER_PORT, NULL );

	if( bResult )
	{
        connectText->setEnabled( false );
        nickText->setEnabled( false );
	}
	else
		connectText->setText( "" );

	return true;
}

bool GameGUI::Connect_Quit( const CEGUI::EventArgs &args )
{
	GameCore::mGraphicsCore->shutdown();
	return true;
}

/*-------------------- DEV CONSOLE --------------------*/
void GameGUI::setupConsole()
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	// Load the layout file for connect box
	CEGUI::Window *pLayout = winMgr.loadWindowLayout( "Console.layout" );

	// Add to gui overlay window
	mSheet->addChildWindow( pLayout );

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

void GameGUI::toggleConsole()
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

bool GameGUI::Console_Send( const CEGUI::EventArgs &args )
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
				GameCore::mGraphicsApplication->setWeather(atoi(&szInput[8]) - 1);
			else if (!stricmp(szInput, "benchmark"))
				GameCore::mGraphicsApplication->startBenchmark(0);
			else if (!strnicmp(szInput, "b1 ", 3))
                GameCore::mGraphicsApplication->loadBloom(GameCore::mGraphicsCore->mCamera->getViewport(), 1, atof(&szInput[3]), -1.0f);
			else if (!strnicmp(szInput, "b2 ", 3))
				GameCore::mGraphicsApplication->loadBloom(GameCore::mGraphicsCore->mCamera->getViewport(), 1, -1.0f, atof(&szInput[3]));
			else if (!strnicmp(szInput, "mb ", 3))
				GameCore::mGraphicsApplication->loadMotionBlur(GameCore::mGraphicsCore->mCamera->getViewport(), 1, atof(&szInput[3]));
			else if (!stricmp(szInput, "wireframe on"))
                GameCore::mGraphicsCore->mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
			else if (!stricmp(szInput, "wireframe off"))
                GameCore::mGraphicsCore->mCamera->setPolygonMode(Ogre::PM_SOLID);
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
        }
    }

    inputText->setText( "" );

	//if( !stricmp( szInput, "exit" ) )
	//	mWinMgr.getWindow( "/Console" )->hide();

	return true;
}

bool GameGUI::Console_Off( const CEGUI::EventArgs &args )
{
	CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();
	mWinMgr.getWindow( "/Console" )->hide();
	return true;
}

/*-------------------- DEV Chatbox --------------------*/
void GameGUI::setupChatbox()
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	// Load the layout file for connect box
	CEGUI::Window *pLayout = winMgr.loadWindowLayout( "Chatbox.layout" );

	// Add to gui overlay window
	mSheet->addChildWindow( pLayout );

	// Get handles to some of the objects
	CEGUI::Window *chatboxFrame = winMgr.getWindow( "/Chatbox" );
	CEGUI::Window *inputText = winMgr.getWindow( "/Chatbox/input" );

	inputText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, 
		CEGUI::Event::Subscriber( &GameGUI::Chatbox_Send, this ) );

	CEGUI::MouseCursor::getSingleton().show();
	inputText->hide();
}

void GameGUI::toggleChatbox()
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

bool GameGUI::Chatbox_Send( const CEGUI::EventArgs &args )
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

void GameGUI::chatboxAddMessage( const char *szNickname, char *szMessage )
{
    char szBuffer[256];
    sprintf( szBuffer, "[colour='FFED9DAA']%s :[colour='FFFFFFFF'] %s", szNickname, szMessage );

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

/*-------------------- SPEEDOMETER --------------------*/
/// @brief Draws the speedo on-screen
void GameGUI::setupSpeedo( void )
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

    Ogre::OverlayElement *hub = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "SPEEDOHUB" );
	hub->setMetricsMode( Ogre::GMM_PIXELS );
	hub->setDimensions( 250, 250 );
	hub->setMaterialName( "speedo_hub" );

	olcSpeedo->addChild( hub );
}

void GameGUI::updateSpeedo( void )
{
    updateSpeedo(GameCore::mPlayerPool->getLocalPlayer()->getCar()->getCarMph(),
                 GameCore::mPlayerPool->getLocalPlayer()->getCar()->getGear());
}

/// @brief	Update the rotation of the speedo needle
/// @param	fSpeed	Float containing speed of car in mph
/// @param  iGear   Current car gear
void GameGUI::updateSpeedo( float fSpeed, int iGear )
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
void GameGUI::setupGearDisplay()
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

void GameGUI::updateCounters()
{
	static char szFPS[64];

	CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/fps" );
	sprintf( szFPS,   "FPS: %.2f", GameCore::mGraphicsCore->mWindow->getAverageFPS());
	fps->setText( szFPS );
}

void GameGUI::setupDamageDisplay()
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
    
	updateDamage();
}

void GameGUI::updateDamage()
{
	oleDamage       ->setMaterialName( "damage_y_body" );
	oleDamageEngine ->setMaterialName( "damage_y_engine" );
    oleDamageFL     ->setMaterialName( "damage_g_fl" );
    oleDamageFR     ->setMaterialName( "damage_r_fr" );
    oleDamageRL     ->setMaterialName( "damage_y_rl" );
    oleDamageRR     ->setMaterialName( "damage_g_rr" );
}

void GameGUI::setupOverlays()
{
    setupSpeedo();
    setupGearDisplay();
    updateSpeedo(0.0f, 0);
    
    setupDamageDisplay();
    updateDamage();
}