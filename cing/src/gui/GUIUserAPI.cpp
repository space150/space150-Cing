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

#include "GUIUserAPI.h"
#include "GUIManagerCEGUI.h"

namespace Cing
{

/**
 * @brief Shows or hides the debut output window
 *
 * @param show if true it will be shown, if false it will be hidden
 */
void showDebugOutput( bool show /*= true*/ )
{
	GUIManagerCEGUI::getSingleton().getDebugOutput().setVisible( show );
}

/**
 * @brief Clears the messages that contains the debug output
 */
void clearDebugOutput()
{
	GUIManagerCEGUI::getSingleton().getDebugOutput().clear();
}

} // namespace Cing