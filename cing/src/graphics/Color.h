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

#ifndef _Cing_Color_h_
#define _Cing_Color_h_

// Precompiled headers
#include "Cing-Precompiled.h"

#include "GraphicsPrereqs.h"

// Ogre
#include "OgreColourValue.h"

namespace Cing
{

/**
 * @brief Stores the information about a color. The color range can be any, but by default is 0.255.
 * The default color model is RGB (red green and blue), and a fourth value, alpha, can be used to represent transparency
 *
 * This color translates from any color range, to 0..1 range that Ogre graphics engine understands, and also
 * provides an interface for al color related operations (such as change of color mode)
 */
class Color: public Ogre::ColourValue
{
public:

	// Constructor: Transforms color to Ogre range
	Color( float gray = 255 );
	Color( float gray, float alpha );
	Color( float red,  float green, float blue );
	Color( float red,  float green, float blue, float alpha );

	Color( int gray );
	Color( int gray, float alpha );

	Color( Ogre::ColourValue color );

	// To set values or range
	void	set	( float red, float green, float blue, float alpha = 255.0f );

	// Getters
	float	getHighRange() const { return m_hightRange; }
	float	getLowRange() const { return m_lowRange; }

	// Color conversion
	Color	normalized	() const;
	void	normalize		();

	// Color mode
	static void							  colorMode	( GraphicsType mode, float range1, float range2, float range3, float range4 );
	static const GraphicsType	getColorMode(){ return m_colorMode; };

	// TODO
	// Operators

private:

	// Attributes
	float m_lowRange, m_hightRange; // Color values range. Default 0..255

	// Static attributes
	static GraphicsType	m_colorMode;
	static float				m_rRange;
	static float				m_gRange;
	static float				m_bRange;
};

} // namespace Cing

#endif // _Color_h_
