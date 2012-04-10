#include "stdafx.h"
#include "GameIncludes.h"


// @brief   Constructor
Lobby::Lobby()
{
}


// @brief   Deconstructor
Lobby::~Lobby()
{
}




void Lobby::setupLobby (const int screenWidth, const int screenHeight)
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
	mSheet->addChildWindow(pLayout);
    
	// Get handles for the buttons
	CEGUI::Window* connectButton = winMgr.getWindow("/Lobby/Servers/btnConnect");
    CEGUI::Window* refreshButton = winMgr.getWindow("/Lobby/Servers/btnRefresh");
    CEGUI::Window* enterIPButton = winMgr.getWindow("/Lobby/Servers/btnIP");
	CEGUI::Window* ip_ipEditBox     = winMgr.getWindow("/Lobby/IPWindow/edtIP");
    CEGUI::Window* ip_connectButton = winMgr.getWindow("/Lobby/IPWindow/btnConnect");
    CEGUI::Window* ip_closeButton   = winMgr.getWindow("/Lobby/IPWindow/btnClose");

	// Register callbacks for the buttons
	connectButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::lobbyConnectPressed, this));
    refreshButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::lobbyRefreshPressed, this));
    enterIPButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::lobbyEnterIPPressed, this));
    ip_connectButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::lobbyIP_connectPressed, this));
    ip_closeButton->subscribeEvent(  CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Lobby::lobbyIP_closePressed, this));
    
    // Add the coulmns to the list
    CEGUI::MultiColumnList* mcl = static_cast<CEGUI::MultiColumnList*>(winMgr.getWindow("/Lobby/Servers/List"));
    mcl->addColumn("Server Name", 0, CEGUI::UDim(0.40f, 0));
    mcl->addColumn("#Players",    1, CEGUI::UDim(0.20f, 0));
    mcl->addColumn("Map",         2, CEGUI::UDim(0.37f, 0));
}


void Lobby::lobbyAddServer (const char* serverName, const char* numberOfPlayers, const char* serverMap, bool selected)
{
    // Get the list.
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::MultiColumnList* mcl = static_cast<CEGUI::MultiColumnList*>(winMgr.getWindow("/Lobby/Servers/List"));

    // Add a row
    unsigned int row = mcl->addRow();
    CEGUI::ListboxTextItem* field;
    // Column 1
    field = new CEGUI::ListboxTextItem(serverName, 0);
    mcl->setItem(field, 0, row);
    field->setSelected(selected);
    field->setSelectionBrushImage("GWEN", "Input.ListBox.EvenLineSelected");
    // Column 2
    field = new CEGUI::ListboxTextItem(numberOfPlayers, 0);
    mcl->setItem(field, 1, row);
    field->setSelected(selected);
    field->setSelectionBrushImage("GWEN", "Input.ListBox.EvenLineSelected");
    // Column 3
    field = new CEGUI::ListboxTextItem(serverMap, 0);
    mcl->setItem(field, 2, row);
    field->setSelected(selected);
    field->setSelectionBrushImage("GWEN", "Input.ListBox.EvenLineSelected");
}

void Lobby::closeLobby (void)
{
    mSheet->removeChildWindow("/Lobby");
	CEGUI::MouseCursor::getSingleton().hide();
}

bool Lobby::lobbyConnectPressed (const CEGUI::EventArgs &args)
{
    OutputDebugString("boobs\n");
    return true;
}

bool Lobby::lobbyRefreshPressed (const CEGUI::EventArgs &args)
{
    OutputDebugString("noobs\n");
    return true;
}

bool Lobby::lobbyEnterIPPressed (const CEGUI::EventArgs &args)
{
    return true;
}

bool Lobby::lobbyIP_connectPressed (const CEGUI::EventArgs &args)
{
    return true;
}

bool Lobby::lobbyIP_closePressed (const CEGUI::EventArgs &args)
{
    return true;
}