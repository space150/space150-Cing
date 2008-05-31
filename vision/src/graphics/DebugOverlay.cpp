/*
  This source file is part of the Vision project
  For the latest info, see http://www.XXX.org

  Copyright (c) 2008 XXX

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

#include "DebugOverlay.h"
#include "Font.h"


// Ogre
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include "OgreTextAreaOverlayElement.h"

namespace Graphics
{

// Static member definition
const std::string DebugOverlay::DEFAULT_DEBUG_PANEL_NAME    = "DebugPanel";
const std::string DebugOverlay::DEFAULT_DEBUG_TEXTAREA_NAME = "DebugTextArea";

/**
 * @internal
 * @brief Constructor. Initializes class attributes.
 */
DebugOverlay::DebugOverlay():
  m_bIsValid( false )
{
}

/**
 * @internal
 * @brief Destructor. Class release.
 */
DebugOverlay::~DebugOverlay()
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
bool DebugOverlay::init()
{
  // Check if the class is already initialized
  if ( isValid() )
    return true;

  // Get the overlay manager
  Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

  // Create the panel to show the debug info 
  Ogre::OverlayContainer* panel = static_cast< Ogre::OverlayContainer* >( overlayMgr.createOverlayElement( "Panel", DEFAULT_DEBUG_PANEL_NAME ) );
  panel->setMetricsMode( Ogre::GMM_PIXELS );

  // Set panel properties
  // TODO poner defaults
  panel->setPosition( 10, 10 );
  panel->setDimensions( 300, 120 );

  // Create a text area to show the debug text
  Ogre::TextAreaOverlayElement* textArea = static_cast< Ogre:: TextAreaOverlayElement* >( overlayMgr.createOverlayElement( "TextArea", DEFAULT_DEBUG_TEXTAREA_NAME ) );
  textArea->setMetricsMode(Ogre::GMM_PIXELS);
  textArea->setPosition(0, 0);
  textArea->setDimensions(300, 120);
  textArea->setCharHeight(26);

  // set the font name to the font resource that you just created.
  textArea->setFontName( Font::DEFAULT_FONT_NAME );

  // say something
  textArea->setCaption( "Hello, World!" ); 

  // Create an overlay, and add the panel
  Ogre::Overlay* overlay = overlayMgr.create( "Overlay" );
  overlay->add2D(panel);

  // Add the text area to the panel
  panel->addChild(textArea);

  // Show the overlay
  overlay->show();

	// The class is now initialized
	m_bIsValid = true;

	return true;
}

/**
 * @internal
 * @brief Releases the class resources. 
 * After this method is called the class is not valid anymore.
 */
void DebugOverlay::end()
{
  // Check if the class is already released
  if ( !isValid() )
    return;

	// The class is not valid anymore
	m_bIsValid = false;
}

/**
 * @internal
 * @brief Updates the class state
 */
void DebugOverlay::update()
{

}

} // namespace Graphics