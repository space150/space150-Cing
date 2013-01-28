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

#include "LightingUserAPI.h"
#include "GraphicsManager.h"

// Ogre
#include "OgreSceneManager.h"

// Framework
#include "framework/Application.h"

namespace Cing
{

/**
 * @brief Sets the ambient light of the scene. The ambient light affects all objects in the scene from all directions
 * Range 0..255
 *
 * @param gray Gray value for the ambient light, this is, red, green and blue are equal
 */
void ambientLight( float gray )
{
	// Check application correctly initialized (could not be if the user didn't calle size function)
	Application::getSingleton().checkSubsystemsInit();

	GraphicsManager::getSingleton().getSceneManager().setAmbientLight( Color(gray , gray , gray).normalized() );
}

/**
 * @brief Sets the ambient light of the scene. The ambient light affects all objects in the scene from all directions.
 * Range 0..255
 *
 * @param red		Red value for the ambient light
 * @param green Green value for the ambient light
 * @param blue	Blue value for the ambient light
 */
void ambientLight( float red, float green, float blue )
{
	// Check application correctly initialized (could not be if the user didn't calle size function)
	Application::getSingleton().checkSubsystemsInit();

	GraphicsManager::getSingleton().getSceneManager().setAmbientLight( Color(red , green , blue).normalized() );
}

/**
 * @brief Sets the ambient light of the scene. The ambient light affects all objects in the scene from all directions.
 * Range 0..255
 *
 * @param color	Color value for the ambient light
 */
void ambientLight( const Color& color )
{
	// Check application correctly initialized (could not be if the user didn't calle size function)
	Application::getSingleton().checkSubsystemsInit();

	// Ogre color range is 0..1, so if this is not the range of the received variable, we need to normalize it
	if ( (!equal(color.getLowRange(), 0.0f)) || (equal(color.getHighRange(), 255.0f)) )
		GraphicsManager::getSingleton().getSceneManager().setAmbientLight( color.normalized() );
	else
		GraphicsManager::getSingleton().getSceneManager().setAmbientLight( color );
}

/**
 * @brief Returns the current value of the ambient light
 * @param red		Red value for the ambient light
 * @param green Green value for the ambient light
 * @param blue	Blue value for the ambient light
 */
Color getAmbientLight()
{
	// Check application correctly initialized (could not be if the user didn't calle size function)
	Application::getSingleton().checkSubsystemsInit();

	Color ambient = GraphicsManager::getSingleton().getSceneManager().getAmbientLight();
	return ambient;
}


} // namespace Cing