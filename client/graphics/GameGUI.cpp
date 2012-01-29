#include "stdafx.h"
#include "GameGUI.h"
#include "GraphicsCore.h"

GameGUI::GameGUI()
{
}

GameGUI::~GameGUI()
{
}

void GameGUI::setupGUI()
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
	mph->setSize( CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    mph->setVisible( false );

	CEGUI::System::getSingleton().setGUISheet( mSheet );

	mSheet->addChildWindow( mph );
}

/*-------------------- CONNECTION BOX --------------------*/

void GameGUI::displayConnectBox()
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
void GameGUI::displayConsole()
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

	inputText->setText( "" );

	if( !stricmp( szInput, "exit" ) )
		mWinMgr.getWindow( "/Console" )->hide();

	return true;
}

bool GameGUI::Console_Off( const CEGUI::EventArgs &args )
{
	CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();
	mWinMgr.getWindow( "/Console" )->hide();
	return true;
}

/*-------------------- DEV Chatbox --------------------*/
void GameGUI::displayChatbox()
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