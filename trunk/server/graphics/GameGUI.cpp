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
	CEGUI::Window* inputText   = winMgr.getWindow("/Server/input");
    CEGUI::Window* adminWindow = winMgr.getWindow("/Server/admin");
	CEGUI::Window* healthText  = winMgr.getWindow("/Server/admin/edtHealth");
	inputText->subscribeEvent(  CEGUI::Editbox::EventTextAccepted,     CEGUI::Event::Subscriber(&GameGUI::receiveFromConsole, this));
    adminWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&GameGUI::closeAdminWindow,   this));
	healthText->subscribeEvent( CEGUI::Editbox::EventTextAccepted,     CEGUI::Event::Subscriber(&GameGUI::healthTextChanged,  this));

    
    // Display the mouse, give the input box focus and hide the admin box.
	CEGUI::MouseCursor::getSingleton().show();
	winMgr.getWindow( "/Server/input" )->activate();
    winMgr.getWindow( "/Server/admin" )->hide();
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
        outputToConsole("spawn normal [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']    Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with the normal difficulty.\n");
        outputToConsole("spawn hard [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10']    Spawns [font='DejaVuMonoItalic-10']X[font='DejaVuMono-10'] AI players with the flee hard difficulty.\n");
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
		    GameCore::mAiCore->createNewAiAgent(seek, easy);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+11)));
    }
    else if( !strnicmp( inputChars, "spawn normal", 12) )
    {
        for (int i = 0; i < atoi((inputChars+13)); i++)
			GameCore::mAiCore->createNewAiAgent(seek, normal);
        outputToConsole("Spawned %d AI players.\n", atoi((inputChars+13)));
    }
    else if( !strnicmp( inputChars, "spawn hard", 10) )
    {
        for (int i = 0; i < atoi((inputChars+11)); i++)
		    GameCore::mAiCore->createNewAiAgent(seek, hard);
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
		openAdminWindow();
		outputToConsole("Admin window opened.\n");
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


/*-------------------- ADMIN WINDOW --------------------*/
void GameGUI::openAdminWindow (void)
{
	CEGUI::WindowManager& winMgr      = CEGUI::WindowManager::getSingleton();
    CEGUI::FrameWindow*   adminWindow = static_cast<CEGUI::FrameWindow*>(winMgr.getWindow("/Server/admin"));

	//add each player to the combobox
	updatePlayerComboBox();
    adminWindow->show();
}

bool GameGUI::closeAdminWindow (const CEGUI::EventArgs &args)
{
	CEGUI::WindowManager& winMgr      = CEGUI::WindowManager::getSingleton();
    CEGUI::FrameWindow*   adminWindow = static_cast<CEGUI::FrameWindow*>(winMgr.getWindow("/Server/admin"));

    adminWindow->hide();
    giveConsoleFocus();

	outputToConsole("Admin window closed.\n");

    return true;
}

//updates the list of players in the combo box
void GameGUI::updatePlayerComboBox()
{
	CEGUI::WindowManager& winMgr         = CEGUI::WindowManager::getSingleton();
    CEGUI::Combobox*      playerComboBox = static_cast<CEGUI::Combobox*>(winMgr.getWindow("/Server/admin/cmbPlayers"));
    CEGUI::Editbox*       healthTextBox  = static_cast<CEGUI::Editbox*>( winMgr.getWindow("/Server/admin/edtHealth"));
    CEGUI::Editbox*       playerTextBox  = static_cast<CEGUI::Editbox*>( winMgr.getWindow("/Server/admin/edtPlayer"));

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
			CEGUI::ListboxItem* playerItem = new CEGUI::ListboxTextItem("[colour='FF000000']" + CEGUI::String(player->getNickname()), 1);
			playerComboBox->addItem(playerItem);
		}
	}

	//now update the player selected box
	CEGUI::ListboxItem* listItem = playerComboBox->getSelectedItem();
	if(listItem)
	{
		//get the player
		CEGUI::String playerName = listItem->getText().substr(19, listItem->getText().length() - 19);
		Player* player = GameCore::mPlayerPool->getPlayer(playerName.c_str());
		//add the text to the player selected window
		playerTextBox->setText(playerName);
        playerComboBox->setText(playerName);
		//playerSelected->show();

		char hp[5];
		//update the health box doesn't have input focus
		if(healthTextBox->hasInputFocus() == false)
		{
			//display the health
			sprintf(hp, "%d", player->getHP());
			healthTextBox->setText(hp);
		}
	}

}

bool GameGUI::healthTextChanged(const CEGUI::EventArgs &args)
{
	CEGUI::WindowManager& winMgr         = CEGUI::WindowManager::getSingleton();
    CEGUI::Combobox*      playerComboBox = static_cast<CEGUI::Combobox*>(winMgr.getWindow("/Server/admin/cmbPlayers"));
    CEGUI::Editbox*       healthTextBox  = static_cast<CEGUI::Editbox*>( winMgr.getWindow("/Server/admin/edtHealth"));
    CEGUI::Editbox*       playerTextBox  = static_cast<CEGUI::Editbox*>( winMgr.getWindow("/Server/admin/edtPlayer"));

	//validate health in editbox
	const char* newHealth = healthTextBox->getText().c_str();
	int len = strlen(newHealth);
	for(int i = 0;i < len;i++)
	{
		if(isdigit(newHealth[i]) == false)
			return false;
	}

	//set the new health
	Player *player = GameCore::mPlayerPool->getPlayer(playerTextBox->getText().c_str());
	int healthInt;
	std::stringstream(newHealth) >> healthInt;
	player->setHP(healthInt);

    return true;
}