#include "stdafx.h"
#include "Lobby.h"
#include "GameCore.h"


void Lobby::setup (const int screenWidth, const int screenHeight)
{
    // Add the splashimage to the background
    /*Ogre::Overlay* lobbyBackground = Ogre::OverlayManager::getSingleton().create("OVERLAY_LOBBY");
    lobbyBackground->setZOrder(501);
    lobbyBackground->show();
    // Create an overlay container and add the splash screen to it.
    Ogre::OverlayContainer* lobbyBackgroundImage = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "LobbyBg"));
    lobbyBackgroundImage->setMetricsMode(Ogre::GMM_PIXELS);
    lobbyBackgroundImage->setHorizontalAlignment(Ogre::GHA_LEFT);
    lobbyBackgroundImage->setVerticalAlignment(Ogre::GVA_TOP);
    lobbyBackgroundImage->setDimensions(screenWidth, screenHeight);
    lobbyBackgroundImage->setMaterialName("CollisionDomain/ClientSplash");
    lobbyBackgroundImage->setPosition(0, 0);
    lobbyBackground->add2D(lobbyBackgroundImage);*/

    // Get the CEGUI window manager
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	// Load the lobby's layout file
	CEGUI::Window* pLayout = winMgr.loadWindowLayout("Lobby.layout");

	// Add to gui overlay window
	mGUIWindow->addChildWindow(pLayout);
    
	// Get handles for the buttons
	CEGUI::Window* connectButton = winMgr.getWindow("/Lobby/Servers/btnConnect");
    CEGUI::Window* refreshButton = winMgr.getWindow("/Lobby/Servers/btnRefresh");
    CEGUI::Window* enterIPButton = winMgr.getWindow("/Lobby/Servers/btnIP");
	CEGUI::Window* ip_ipEditBox     = winMgr.getWindow("/Lobby/IPWindow/edtIP");
    CEGUI::Window* ip_connectButton = winMgr.getWindow("/Lobby/IPWindow/btnConnect");
    CEGUI::Window* ip_closeButton   = winMgr.getWindow("/Lobby/IPWindow/btnClose");

	// Register callbacks for the buttons
	connectButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::connectPressed, this));
    refreshButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::refreshPressed, this));
    enterIPButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::enterIPPressed, this));
    ip_connectButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::IP_connectPressed, this));
    ip_closeButton->subscribeEvent(  CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::IP_closePressed, this));
    
    // Add the coulmns to the list
    CEGUI::MultiColumnList* mcl = static_cast<CEGUI::MultiColumnList*>(winMgr.getWindow("/Lobby/Servers/List"));
    mcl->addColumn("Server Name", 0, CEGUI::UDim(0.40f, 0));
    mcl->addColumn("Players",     1, CEGUI::UDim(0.20f, 0));
    mcl->addColumn("Arena",       2, CEGUI::UDim(0.37f, 0));
}


void Lobby::addServer (std::string serverName, std::string numberOfPlayers, std::string serverMap, bool selected)
{
    // Get the list.
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::MultiColumnList* mcl = static_cast<CEGUI::MultiColumnList*>(winMgr.getWindow("/Lobby/Servers/List"));

    // Add a row
    unsigned int row = mcl->addRow();
    CEGUI::ListboxTextItem* field;
    // Column 1
    field = new CEGUI::ListboxTextItem("[colour='FF000000']" + serverName, 0);
    mcl->setItem(field, 0, row);
    field->setSelected(selected);
    field->setSelectionBrushImage("GWEN", "Input.ListBox.EvenLineSelected");
    // Column 2
    field = new CEGUI::ListboxTextItem("[colour='FF000000']" + numberOfPlayers, 0);
    mcl->setItem(field, 1, row);
    field->setSelected(selected);
    field->setSelectionBrushImage("GWEN", "Input.ListBox.OddLineSelected");
    // Column 3
    field = new CEGUI::ListboxTextItem("[colour='FF000000']" + serverMap, 0);
    mcl->setItem(field, 2, row);
    field->setSelected(selected);
    field->setSelectionBrushImage("GWEN", "Input.ListBox.EvenLineSelected");
}

void Lobby::close (void)
{
    mGUIWindow->removeChildWindow("/Lobby");
	CEGUI::MouseCursor::getSingleton().hide();
}

bool Lobby::connectPressed (const CEGUI::EventArgs &args)
{
    // Broadcast for online LAN servers
    GameCore::mNetworkCore->AutoConnect( SERVER_PORT );

    while (GameCore::mNetworkCore->m_szHost == NULL)
    {
        GameCore::mNetworkCore->frameEvent(NULL);
        // Just chillin. Gonna lock up here if a server dont exist lol.
    }

    GameCore::mClientGraphics->loadGame();

    return true;
}

bool Lobby::refreshPressed (const CEGUI::EventArgs &args)
{
    //OutputDebugString("noobs\n");
    return true;
}

bool Lobby::enterIPPressed (const CEGUI::EventArgs &args)
{
    return true;
}

bool Lobby::IP_connectPressed (const CEGUI::EventArgs &args)
{
    return true;
}

bool Lobby::IP_closePressed (const CEGUI::EventArgs &args)
{
    return true;
}
