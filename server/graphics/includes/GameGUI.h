/**
 * @file	GameGUI.h
 * @brief 	Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef GAMEGUI_H
#define GAMEGUI_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "CircularBuffer.h"
#include "NetworkCore.h"
#include "CEGUI.h"
#include <stdio.h>
#include <stdarg.h>

#define COLLISION_DOMAIN_SERVER
#define TIMESTAMP_CONSOLE

class StringCircularBuffer : public CircularBuffer<char*>
{
public:
    /// @brief  Constructor, setting the CircularBuffer's variables.
    /// @param  bs  The buffer size. Once initialised this is not changeable.
    StringCircularBuffer (uint8_t bs = 32, uint16_t sl = 128) : CircularBuffer<char*>(bs), strlen(sl)
    {
        //buffer = (char**) malloc(bufferSize * sizeof(char) * strlen);
        buffer = (char**) malloc(bufferSize * sizeof(char*));
        for (uint8_t i = 0; i < bufferSize; i++)
            buffer[i] = (char*) malloc(strlen * sizeof(char));
    }
    
    /// @brief  Deconstructor.
    ~StringCircularBuffer (void)
    {
        for (uint8_t i = 0; i < bufferSize; i++)
            free(buffer[i]);
        free(buffer);
    }

    virtual void add (const char* item)
    {
        strcpy(buffer[head], item);
        head = sMod(head + 1);
        if (head == tail)
            tail = sMod(tail + 1);
    }

private:
    const uint16_t strlen;
};

class GameGUI
{
public:
    GameGUI (void) : consoleHistory(16), consoleHistoryLocation(0xFF) {}
    ~GameGUI (void) {}
    
	void setupConsole (CEGUI::Window* guiWindow);
    void outputToConsole (const char* str, ...);
    
    // These two methods are called in ServerGraphics.cpp
	void updatePlayerComboBox (void);
    void giveConsoleFocus (void);

    // This is called in Input.cpp
    void loadConsoleHistory (bool reverseLoading);

private:
    bool receiveFromConsole (const CEGUI::EventArgs &args);
    void scrollConsoleToBottom (void);
	bool healthTextChanged (const CEGUI::EventArgs &args);
	void openAdminWindow (void);
	bool closeAdminWindow (const CEGUI::EventArgs &args);

    StringCircularBuffer consoleHistory;
    uint8_t              consoleHistoryLocation;
	//CEGUI::Combobox* playerComboBox;
	//CEGUI::FrameWindow*   playerSelected;
	//CEGUI::Editbox* health;
};

#endif
