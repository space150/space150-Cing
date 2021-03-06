/*
 This source file is part of the Cing project
 For the latest info, see http://www.cing.cc
 
 Copyright (c) 2006-2009 Julio Obelleiro and Jorge Cano
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Precompiled headers
#include "Cing-Precompiled.h"

#include "InputManager.h"

// Graphics
#include "graphics/GraphicsManager.h"

// OIS
#include "OIS.h"

// Ogre
#include "OgreStringConverter.h"

// STL
#include <sstream>

namespace Cing
{
	
	// Static member definition
	Mouse			InputManager::m_mouse;
	Keyboard	InputManager::m_keyboard;
	
	/**
	 * @internal
	 * @brief Constructor. Initializes class attributes.
	 */
	InputManager::InputManager():
	m_pOISInputManager( NULL ),
	m_bIsValid        ( false )
	{
	}
	
	/**
	 * @internal
	 * @brief Destructor. Class release.
	 */
	InputManager::~InputManager()
	{
		// Release resources
		end();
	}
	
	/**
	 * @internal
	 * @brief Initializes the class so it becomes valid.
	 *
	 * @return true if the initialization was ok | false otherwise
	 */
	bool InputManager::init()
	{
		// Check if the class is already initialized
		if ( isValid() )
			return true;
		
		// Variables to init OIS
		OIS::ParamList      paramList;
		size_t              windowHnd = 0;
		std::ostringstream  windowHndStr;
		
		// Fill params depending on the OS
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		{
			//paramList.insert( std::make_pair(std::string( "w32_mouse" ), std::string( "DISCL_BACKGROUND" ) ) );
			//paramList.insert( std::make_pair(std::string( "w32_mouse" ), std::string( "DISCL_NONEXCLUSIVE" ) ) );
			
			paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
			
			
			// TODO: try -> Uncomment these two lines to allow users to switch keyboards via the language bar
			paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND") ));
			paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE") ));
		}
#else
		{	
			paramList.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
			paramList.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
			paramList.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
			paramList.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
		}
#endif
		
		// Get window handle
		windowHnd = GraphicsManager::getSingleton().getMainWindow().getWindowHandle();
		
		// Insert window handle in parameters
		windowHndStr << (unsigned int) windowHnd;
		//paramList.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );   
		paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(windowHnd)));

		
		// Create OIS input manager
		m_pOISInputManager = OIS::InputManager::createInputSystem( paramList );
		
		// Init mouse and keyboard
		m_mouse.init( m_pOISInputManager );
		m_keyboard.init( m_pOISInputManager );
		
		// The class is now initialized
		m_bIsValid = true;
		
		return true;
	}
	
	/**
	 * @internal
	 * @brief Releases the class resources. 
	 * After this method is called the class is not valid anymore.
	 */
	void InputManager::end()
	{
		// Check if the class is already released
		if ( !isValid() )
			return;
		
		// Release keyboad and mouse
		m_mouse.end();
		m_keyboard.end();
		
		// Release OIS input manager
		m_pOISInputManager->destroyInputSystem( m_pOISInputManager );
		m_pOISInputManager = NULL;
		
		
		// The class is not valid anymore
		m_bIsValid = false;
	}
	
	/**
	 * @internal
	 * @brief Updates the class state
	 */
	void InputManager::update()
	{
		// Update input devices
		m_mouse.update();
		m_keyboard.update();
	}
} // namespace Cing