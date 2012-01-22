#include "stdafx.h"
#include "GameGUI.h"
#include "GraphicsCore.h"

GameGUI::GameGUI( CEGUI::OgreRenderer *renderer )
{
	mGuiRenderer = renderer;
}

GameGUI::~GameGUI()
{
}

void GameGUI::setupGUI()
{
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");

	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
    // Font defaulting
    if(CEGUI::FontManager::getSingleton().isDefined("DejaVuSans-10"))
    {
		CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
    }

	mSheet = CEGUI::WindowManager::getSingleton().
		createWindow( "DefaultWindow", "root_wnd" );

	CEGUI::System::getSingleton().setGUISheet( mSheet );
}

void GameGUI::displayConnectBox()
{
	CEGUI::Window *pConLayout = 
		CEGUI::WindowManager::getSingleton().loadWindowLayout( "Connect.layout" );

	mSheet->addChildWindow( pConLayout );

	CEGUI::Window *hostText = 
		CEGUI::WindowManager::getSingleton().getWindow( "/Connect/host" );

	hostText->subscribeEvent( CEGUI::Editbox::EventTextAccepted, 
		CEGUI::Event::Subscriber( &GameGUI::Connect_Host, this ) );
}

bool GameGUI::Connect_Host( const CEGUI::EventArgs &args )
{
	CEGUI::WindowManager& mWinMgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Editbox *connectText = 
		static_cast<CEGUI::Editbox*> ( mWinMgr.getWindow( "/Connect/host" ) );

	// This is THE MOST retarded way of accessing the network possible..
	bool bResult = NetworkCore::mGraphics->mNetworkCore->Connect( 
		connectText->getText().c_str(), SERVER_PORT, NULL );

	if( bResult )
		mSheet->removeChildWindow( "Root" );
	else
		connectText->setText( "" );

	return true;
}