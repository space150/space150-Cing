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

#include "Color.h"

// Common
#include "common/MathUtils.h"

namespace Cing
{

/// Static member initialization
	GraphicsType Color::m_colorMode = RGB;
	float  Color::m_rRange = 255.0f ;
	float  Color::m_gRange = 255.0f ;
	float  Color::m_bRange = 255.0f ;

/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param gray Gray color. Red, green and blue with the same value
 */
Color::Color( float gray /*= 255*/ ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	set( gray, gray, gray, m_hightRange );
}


/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param gray	Gray color. Red, green and blue with the same value
 * @param alpha Alpha value. Represents transparency. Max value means opaque, min value means transparent.
 */
Color::Color( float gray, float alpha ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	set( gray, gray, gray, alpha );
}

/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param red		Red color value
 * @param green	Green color value
 * @param blue	Blue color value
 */
Color::Color( float  red, float green, float blue ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	set( red, green, blue, m_hightRange );
}

/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param red		Red color value
 * @param green	Green color value
 * @param blue	Blue color value
 * @param alpha Alpha value. Represents transparency. Max value means opaque, min value means transparent.
 */
Color::Color( float  red, float green, float blue, float alpha ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	set( red, green, blue, alpha );
}

/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param gray Gray color. Red, green and blue with the same value
 */
Color::Color( int gray ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	set( (float)gray, (float)gray, (float)gray, m_hightRange );
}

/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param gray Gray color.	Red, green and blue with the same value
 * @param alpha				Transparency value (0 = transparent, 255 = opaque)
 */
Color::Color( int gray, float alpha ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	set( (float)gray, (float)gray, (float)gray, alpha );
}

/**
 * @brief Initializes the color with a specific color values (depending on the color range
 * which by default is 0..255, and RGB color model (red, green, blue).
 *
 * @param red		Red color value
 * @param green	Green color value
 * @param blue	Blue color value
 * @param alpha Alpha value. Represents transparency. Max value means opaque, min value means transparent.
 */
Color::Color(  Ogre::ColourValue color ): 
	m_lowRange( 0.0f ), 
	m_hightRange( 255.0f )
{
	color.r = map( color.r, 0.0, 1.0, 0, 255);
	color.g = map( color.g, 0.0, 1.0, 0, 255);
	color.b = map( color.b, 0.0, 1.0, 0, 255);
	
	set( color.r, color.g, color.b, color.a );
}
/**
 * @brief Sets the color values of this Color object.
 *
 * @param red		Red color value
 * @param green	Green color value
 * @param blue	Blue color value
 * @param alpha Alpha value. Represents transparency. Max value means opaque, min value means transparent.
 */
void Color::set( float red, float green, float blue, float alpha /*= 255.0f*/ )
{
	// store values
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

/**
 * @internal 
 * @brief Returns the color value normalized. This means, value will range from 0..1
 *
 * @return Normalized (0..1) color value
 */
Color Color::normalized() const
{
	// Create an equal color
	Color normalizedColor = *this;

	// Normalize it
	normalizedColor.normalize();

	return normalizedColor;
}

/**
 * @internal 
 * @brief Normalizes the values of this color to range 0..1
 */
void Color::normalize()
{
	// Convert values to current color range
	r = map( r, m_lowRange, m_hightRange, 0.0f, 1.0f );
	g = map( g, m_lowRange, m_hightRange, 0.0f, 1.0f );
	b = map( b, m_lowRange, m_hightRange, 0.0f, 1.0f );
	a = map( a, m_lowRange, m_hightRange, 0.0f, 1.0f );
}

/**
 * @internal 
 * @brief Change the color mode and range
 */
void Color::colorMode( GraphicsType mode, float range1, float range2, float range3, float range4 )
{
	m_colorMode  = mode;
	//m_lowRange   = 0;
	//m_hightRange = 255;
}

} // namespace Cing