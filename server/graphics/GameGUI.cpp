#include "stdafx.h"
#include "GameGUI.h"

GameGUI::GameGUI() : consoleHistory(16), consoleHistoryLocation(0xFF)
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
	pFont = &CEGUI::FontManager::getSingleton().create("DejaVuMono-10.font");
	pFont->setProperty( "PointSize", "12" );
	pFont = &CEGUI::FontManager::getSingleton().create("DejaVuMonoItalic-10.font");
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

	// Get a handle to the input box
	CEGUI::Window *inputText = winMgr.getWindow( "/Server/input" );
	inputText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber( &GameGUI::receiveFromConsole, this ) );

    // Display the mouse and give the input box focus.
	CEGUI::MouseCursor::getSingleton().show();
	winMgr.getWindow( "/Server/input" )->activate();
}

bool GameGUI::receiveFromConsole( const CEGUI::EventArgs &args )
{
	CEGUI::WindowManager& winMgr        = CEGUI::WindowManager::getSingleton();
	CEGUI::Editbox*       inputText     = static_cast<CEGUI::Editbox*>(winMgr.getWindow("/Server/input"));
    CEGUI::DefaultWindow* consoleBuffer = static_cast<CEGUI::DefaultWindow*>(winMgr.getWindow("/Server/buffer"));
    CEGUI::Scrollbar*     cbScroll      = static_cast<CEGUI::Scrollbar*>(winMgr.getWindow("/Server/buffer__auto_vscrollbar__"));

    // Get the input text, echo it back to the console (unless its blank, in which case return, or a chat message in which case it doesn't
    // make sense to have the message printed twice so don't echo) and clear the input box.
	const char* inputChars = inputText->getText().c_str();
    if (inputChars[0] == 0)
        return true;
    if (inputChars[0] != '@')
        outputToConsole("> %s\n", inputChars);
	inputText->setText("");

    // Parse the string and execute the required action
    if ( inputChars[0] == '@' )
    {
        outputToConsole("[colour='FF6DDD77']Admin:[colour='FFFFFFFF'] %s\n", (inputChars+1));
        GameCore::mNetworkCore->sendChatMessage((inputChars+1));
    }
    else if ( !stricmp( inputChars,  "help" ) )
    {
        //              "---------------------MAX STRING LENGTH TO AVOID OVERFLOW--------------------"
        outputToConsole("List of server commands:\n");
        outputToConsole("help            Displays this list.\n");
        outputToConsole("@[font='DejaVuMonoItalic-10']STRING[font='DejaVuMono-10']         Sends [font='DejaVuMonoItalic-10']STRING[font='DejaVuMono-10'] as an admin message to players.\n");
        outputToConsole("prep            Places players in a ring and resets their health.\n");
        outputToConsole("start           Starts the game.\n");
        outputToConsole("ddinfo          Draws the death info.\n");
        outputToConsole("kill [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']          Kills player [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] (where [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] is their integer player index).\n");
        outputToConsole("spawn wander [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']  Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with the wander mechanic.\n");
        outputToConsole("spawn seek [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']    Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with the seek mechanic.\n");
        outputToConsole("spawn flee [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']    Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with the flee mechanic.\n");
    }
	else if( !stricmp( inputChars,  "prep" ) )
    {
		GameCore::mGameplay->preparePlayers();
        outputToConsole("Prepared players.\n");
    }
	else if( !stricmp( inputChars,  "start" ) )
    {
		GameCore::mGameplay->startGame();
        outputToConsole("Game started.\n");
    }
	else if( !stricmp( inputChars,  "ddinfo" ) )
    {
		GameCore::mGameplay->drawDeathInfo();
        outputToConsole("Drawn death info.\n");
    }
    else if( !strnicmp( inputChars, "kill", 4) )
    {
        GameCore::mPlayerPool->getPlayer(atoi((inputChars+5)))->killPlayer();
        outputToConsole("Killed player %d.\n", atoi((inputChars+5)));
    }
    else if( !strnicmp( inputChars, "spawn wander", 12) )
    {
        for (int i = 0; i < atoi((inputChars+13)); i++)
		    GameCore::mAiCore->createNewAiAgent(wander);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+13)));
    }
    else if( !strnicmp( inputChars, "spawn seek", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(seek);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+11)));
    }
    else if( !strnicmp( inputChars, "spawn flee", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(flee);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+11)));
    }
    else
    {
        outputToConsole("Unrecognised command.\n");
    }

    // Add the input to the history and clear the history position (so you start from the most recent again).
    consoleHistory.add(inputChars);
    consoleHistoryLocation = 0xFF;

    // Scroll to the bottom of the pane.
    if (cbScroll != NULL)
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

	CEGUI::WindowManager& winMgr        = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow* consoleBuffer = static_cast<CEGUI::DefaultWindow*>(winMgr.getWindow("/Server/buffer"));
	consoleBuffer->appendText(CEGUI::String(buffer));
}

void GameGUI::loadConsoleHistory( bool reverseLoading )
{
    // Check there is history
    if (consoleHistory.isEmpty())
        return;
    
    // If true, there is no current history location (i.e. no history is currently loaded)
    if (consoleHistoryLocation == 0xFF)
    {
        // If we have been asked to load the next oldest item, load the first item in history.
        if (reverseLoading)
            consoleHistoryLocation = (consoleHistory.getHead() == 0) ? 16 : consoleHistory.getHead() - 1;
        // If we have been asked to load the next newest item, as we are at the front of the history already do nothing
        else
            return;
    }
    // We are currently looking at history
    else
    {
        // If true, we need to load the next oldest item (reverse chronological order loading).
        if (reverseLoading)
        {
            if (consoleHistoryLocation == consoleHistory.getTail())
                return;
            else
                consoleHistoryLocation = (consoleHistoryLocation == 0) ? 16 : consoleHistoryLocation - 1;
        }
        // Otherwise we need to load the next newest item (forwards chronological order loading).
        else
        {
            if (consoleHistoryLocation == consoleHistory.getHead() - 1)
                consoleHistoryLocation = 0xFF;
            else
                consoleHistoryLocation = (consoleHistoryLocation == 16) ? 0 : consoleHistoryLocation + 1;
        }
    }
        
	CEGUI::WindowManager& winMgr    = CEGUI::WindowManager::getSingleton();
	CEGUI::Editbox*       inputText = static_cast<CEGUI::Editbox*>(winMgr.getWindow("/Server/input"));

    // Load the history into the input box
    if (consoleHistoryLocation == 0xFF)
        inputText->setText("");
    else
	    inputText->setText(*(consoleHistory.get(consoleHistoryLocation)));
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