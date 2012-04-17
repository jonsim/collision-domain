#include "stdafx.h"
#include "GameGUI.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#define strncasecmp strnicmp
	#define strcasecmp  stricmp
#endif



/*-------------------- DEV CONSOLE --------------------*/
void GameGUI::setupConsole (CEGUI::Window* guiWindow)
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	// Load the layout file for connect box and add it to the overlay
	CEGUI::Window* pLayout = winMgr.loadWindowLayout("Server.layout");
	guiWindow->addChildWindow( pLayout );

	// Get a handle to the input box
	CEGUI::Window* inputText = winMgr.getWindow( "/Server/input" );
	inputText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber( &GameGUI::receiveFromConsole, this ) );
    
    // Display the mouse and give the input box focus.
	CEGUI::MouseCursor::getSingleton().show();
	winMgr.getWindow( "/Server/input" )->activate();
}

bool GameGUI::receiveFromConsole (const CEGUI::EventArgs &args)
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
        outputToConsole("spawn easy [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']  Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with easy difficulty.\n");
        outputToConsole("spawn normal [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']    Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with normal difficulty.\n");
        outputToConsole("spawn hard [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']    Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with hard difficulty.\n");
        outputToConsole("get server fps  Returns the server's average fps.\n");
        outputToConsole("get gfx fps     Returns the server's graphics fps.\n");
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
    else if( !strnicmp( inputChars, "spawn easy", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(seek, level::easy);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+11)));
    }
    else if( !strnicmp( inputChars, "spawn normal", 12) )
    {
        for (int i = 0; i < atoi((inputChars+13)); i++)
			GameCore::mAiCore->createNewAiAgent(seek, level::normal);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+13)));
    }
    else if( !strnicmp( inputChars, "spawn hard", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(seek, level::hard);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+11)));
    }
    else if( !stricmp( inputChars, "get server fps" ) )
    {
        outputToConsole("Server's average fps: %.2f.\n", GameCore::mServerGraphics->mAverageFrameRate);
    }
    else if( !stricmp( inputChars, "get gfx fps" ) )
    {
        outputToConsole("Server's graphics average fps: %.2f.\n", GameCore::mServerGraphics->mWindow->getAverageFPS());
    }
	else if( !strcasecmp(inputChars, "admin"))
	{
		outputToConsole("Opening admin window\n");
		openAdminWindow();
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

void GameGUI::outputToConsole (const char* str, ...)
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

void GameGUI::loadConsoleHistory (bool reverseLoading)
{
    // Check there is history
    if (consoleHistory.isEmpty())
        return;
    uint8_t historySize = consoleHistory.getSize();
    
    // If true, there is no current history location (i.e. no history is currently loaded)
    if (consoleHistoryLocation == 0xFF)
    {
        // If we have been asked to load the next oldest item, load the first item in history.
        if (reverseLoading)
            consoleHistoryLocation = historySize - 1;
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
            if (consoleHistoryLocation == 0)
                return;
            else
                consoleHistoryLocation--;
        }
        // Otherwise we need to load the next newest item (forwards chronological order loading).
        else
        {
            if (consoleHistoryLocation == historySize - 1)
                consoleHistoryLocation = 0xFF;
            else
                consoleHistoryLocation++;
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

void GameGUI::giveConsoleFocus (void)
{
    // Give the input box focus.
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.getWindow("/Server/input")->activate();
}

void GameGUI::openAdminWindow()
{
	//create combo box displaying players
	playerComboBox = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/Combobox", (CEGUI::utf8*)"NameID");

	//add each player to the combobox
	updatePlayerComboBox();
 
    playerComboBox->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.25,0.0)));
    playerComboBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.45, 0), CEGUI::UDim(0.25,0.0)));
    playerComboBox->setText("Players");
    playerComboBox->setAlwaysOnTop(true);
    GameCore::mServerGraphics->getGUIWindow()->addChildWindow(playerComboBox);
    playerComboBox->show();

	
	playerSelected = static_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/FrameWindow"));
	GameCore::mServerGraphics->getGUIWindow()->addChildWindow(playerSelected);
	playerSelected->setSize(CEGUI::UVector2(CEGUI::UDim(0.45, 0), CEGUI::UDim(0.25,0.0)));
	playerSelected->setPosition(CEGUI::UVector2(CEGUI::UDim(0.45, 0), CEGUI::UDim(0.45,0.0)));
	playerSelected->setAlwaysOnTop(true);

	//create health box
	CEGUI::Editbox* healthLabel = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/Editbox"));
	healthLabel->setText("Health");
	healthLabel->setReadOnly(true);
	playerSelected->addChildWindow(healthLabel);
	healthLabel->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.25,0.0)));
	healthLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.15,0.0)));
	healthLabel->show();

	health = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/Editbox"));
	playerSelected->addChildWindow(health);
	health->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.25,0.0)));
	health->setPosition(CEGUI::UVector2(CEGUI::UDim(0.45, 0), CEGUI::UDim(0.15,0.0)));
	health->show();

}

//updates the list of players in the combo box
void GameGUI::updatePlayerComboBox()
{
	if(playerComboBox == NULL)
		return;

	//get the list of players
	std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();

	//return if there are no players
	if(players.size() == 0)
		return;
	
	int i = 0;

	if(GameCore::mPlayerPool->getNumberOfPlayers() != playerComboBox->getItemCount())
	{
		playerComboBox->resetList();
		for(std::vector<Player*>::iterator it = players.begin();it != players.end();it++)
		{
			Player* player = (Player*)(*it);
			CEGUI::ListboxItem* playerItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)player->getNickname(), 1);
			playerComboBox->addItem(playerItem);
		}
	}


	//now update the player selected box
	CEGUI::ListboxItem* listItem = playerComboBox->getSelectedItem();
	if(listItem)
	{
		CEGUI::String playerName = listItem->getText();
		const char* nickname = playerName.c_str();
		//now get the player
		Player* player = GameCore::mPlayerPool->getPlayer(nickname);
		//add the text to the player selected window
		playerSelected->setText(playerName);
		playerSelected->show();
		//display the health
		char hp[4];
		sprintf(hp, "%d", player->getHP());
		health->setText(hp);
		health->show();
	}

}