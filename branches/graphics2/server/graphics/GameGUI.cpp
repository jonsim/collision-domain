#include "stdafx.h"
#include "GameGUI.h"

GameGUI::GameGUI()
{
}

GameGUI::~GameGUI()
{
}

void GameGUI::initialiseGUI()
{
	// Load the fonts and set their sizes.
    CEGUI::Font* pFont;
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	pFont = &CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
	pFont->setProperty( "PointSize", "10" );
	pFont = &CEGUI::FontManager::getSingleton().create("monospace-10.font");
	pFont->setProperty( "PointSize", "12" );
	//pFont->setAutoScaled( false );

	// Register font as default
    if(CEGUI::FontManager::getSingleton().isDefined("DejaVuSans-10"))
		CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");

	// Create skin scheme window parameters
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

	// Register skin's default image set and cursor icon
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
	CEGUI::MouseCursor::getSingleton().hide();

	// Tell CEGUI where to look for layouts
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	// Create an empty default window layer
	mSheet = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "root_wnd" );

	CEGUI::System::getSingleton().setGUISheet( mSheet );
}

/*-------------------- DEV CONSOLE --------------------*/
void GameGUI::setupConsole()
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	// Load the layout file for connect box and add it to the overlay
	CEGUI::Window *pLayout = winMgr.loadWindowLayout("Server.layout");
	mSheet->addChildWindow( pLayout );

	// Get handles to some of the objects
	CEGUI::Window *inputText = winMgr.getWindow( "/Server/input" );
	inputText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber( &GameGUI::receiveFromConsole, this ) );

    // Display the mouse and give the input box focus.
	CEGUI::MouseCursor::getSingleton().show();
	winMgr.getWindow( "/Server/input" )->activate();
}

bool GameGUI::receiveFromConsole( const CEGUI::EventArgs &args )
{
	CEGUI::WindowManager&    winMgr        = CEGUI::WindowManager::getSingleton();
	CEGUI::Editbox*          inputText     = static_cast<CEGUI::Editbox*>(winMgr.getWindow("/Server/input"));
	CEGUI::MultiLineEditbox* consoleBuffer = static_cast<CEGUI::MultiLineEditbox*>(winMgr.getWindow("/Server/buffer"));
    CEGUI::Scrollbar*        cbScroll      = consoleBuffer->getVertScrollbar();

    CEGUI::String inputString  = inputText->getText();
    CEGUI::String outputString = inputText->getText();
    outputString.insert(0, "> ", 2);
	char* inputChars = (char*) inputString.c_str();

	consoleBuffer->appendText(outputString);
	inputText->setText("");
    
    if ( !stricmp( inputChars,  "help" ) )
    {
        outputToConsole("List of server commands:");
        outputToConsole("prep            Prepares the players, organising them in a ring and resetting health.");
        outputToConsole("start           Starts the game.");
        outputToConsole("ddinfo          Draws the death info.");
        outputToConsole("kill x          Kills player x (where x is an integer representing their player index).");
        outputToConsole("spawn wander x  Spawns x AI players with the wander mechanic.");
        outputToConsole("spawn seek x    Spawns x AI players with the seek mechanic.");
        outputToConsole("spawn flee x    Spawns x AI players with the flee mechanic.");
    }
	else if( !stricmp( inputChars,  "prep" ) )
    {
		GameCore::mGameplay->preparePlayers();
        outputToConsole("Prepared players.");
    }
	else if( !stricmp( inputChars,  "start" ) )
    {
		GameCore::mGameplay->startGame();
        outputToConsole("Game started.");
    }
	else if( !stricmp( inputChars,  "ddinfo" ) )
    {
		GameCore::mGameplay->drawDeathInfo();
        outputToConsole("Drawn death info.");
    }
    else if( !strnicmp( inputChars, "kill", 4) )
    {
        GameCore::mPlayerPool->getPlayer(atoi((inputChars+5)))->killPlayer();
        outputToConsole("Killed player %d.", atoi((inputChars+5)));
    }
    else if( !strnicmp( inputChars, "spawn wander", 12) )
    {
        for (int i = 0; i < atoi((inputChars+13)); i++)
		    GameCore::mAiCore->createNewAiAgent(wander);
        outputToConsole("Spawned %d AI players.", atoi((inputChars+13)));
    }
    else if( !strnicmp( inputChars, "spawn seek", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(seek);
        outputToConsole("Spawned %d AI players.", atoi((inputChars+11)));
    }
    else if( !strnicmp( inputChars, "spawn flee", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(flee);
        outputToConsole("Spawned %d AI players.", atoi((inputChars+11)));
    }
    else
    {
        outputToConsole("Unrecognised command.");
    }

    // Scroll to the bottom of the pane.
    cbScroll->setScrollPosition(cbScroll->getDocumentSize());

	return true;
}

void GameGUI::outputToConsole( const char* str, ... )
{
    // Provide printf like functionality. I do not know if this is cross platform or not (I suspect it might
    // be compiler specific, in which case I assume it doesn't compile a gcc version will need to be sourced).
    char buffer[256];
    va_list ap;

    if (!str)
        *buffer=0;
    else
    {
        va_start(ap, str);
        vsprintf(buffer, str, ap);
        va_end(ap);
    }

	CEGUI::WindowManager&    winMgr        = CEGUI::WindowManager::getSingleton();
	CEGUI::MultiLineEditbox* consoleBuffer = static_cast<CEGUI::MultiLineEditbox*>(winMgr.getWindow("/Server/buffer"));
	consoleBuffer->appendText(CEGUI::String(buffer));
}

/*-------------------- DEV Chatbox --------------------*
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

	CEGUI::Listbox *lstHistory = 
		static_cast<CEGUI::Listbox*> ( mWinMgr.getWindow( "/Chatbox/buffer" ) );

	CEGUI::ListboxTextItem *newItem;
	if( lstHistory->getItemCount() == 13 )
	{
		newItem = static_cast<CEGUI::ListboxTextItem*> 
			( lstHistory->getListboxItemFromIndex( 0 ) );
		newItem->setAutoDeleted( false );
		lstHistory->removeItem( newItem );
		newItem->setAutoDeleted( true );
		newItem->setText( inputText->getText() );
	}
	else
	{
		newItem = new CEGUI::ListboxTextItem( inputText->getText() );
	}
	
	lstHistory->addItem( newItem );
	lstHistory->ensureItemIsVisible( lstHistory->getItemCount() );

	inputText->setText( "" );

	mWinMgr.getWindow( "/Chatbox/input" )->hide();

	return true;
}*/