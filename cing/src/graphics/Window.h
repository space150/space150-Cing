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

#ifndef _Window_H_
#define _Window_H_

// Precompiled headers
#include "Cing-Precompiled.h"

#include "GraphicsPrereqs.h"
#include "graphics/Color.h"
// OGRE
#include "OgreRenderTarget.h"

namespace Cing
{

/**
 * @internal
 * Represents a rendering window with an associated camera.
 * It is also a wrapper around the Ogre window class.
 */
class Window
{
public:

  // Public structures
  /**
   * @internal
   * Contains the information of the window metrics
   */
  struct TWindowMetrics
  {
    unsigned int  width;
    unsigned int  height;
    unsigned int  colourDepth;
    int           left;
    int           top;

  };

	// Constructor / Destructor
	 Window                   ();
	~Window                   ();

	// Init / Release / Update
	bool                                  	init                  	( Ogre::RenderWindow* pOgreWindow );
	void                                  	end                   	();
	void                                  	update                	();

	// Query methods    
	bool                                  	isValid               	() const { return m_bIsValid; }
	bool                                  	isClosed              	() const;
	bool                                  	isFullScreen          	() const;
	bool									isActive				() const;
	int										getWidth				() const { return m_width; }
	int										getHeight				() const { return m_height; }
	void                                  	getMetrics            	( TWindowMetrics& metrics ) const;
	size_t                                	getWindowHandle       	() const;
	const Ogre::RenderTarget::FrameStats& 	getFrameStats         	() const { return *m_stats; }
	Ogre::RenderWindow*					  	getOgreWindow			() { return m_pOgreWindow; }
	const Ogre::ColourValue&			  	getBackgroundColor		();
	Ogre::Viewport*							getMainViewport			() { return m_mainViewport; }						
	Ogre::Viewport*							getViewport				( int index = 0 );
	
	// Various
	void                                  	attachCameraToWindow  ( Camera3D& camera, int viewportIndex = 0 );
	void                                  	attachCameraToWindow  ( Ogre::Camera* ogreCamera, int viewportIndex = 0);
	void                                  	setBackgroundColor    ( const Color& color );

private:

	// Attributes
	Ogre::RenderWindow*	                  	m_pOgreWindow;  		///< Ogre window
	Ogre::Viewport*                       	m_mainViewport; 		///< Window viewport
	const Ogre::RenderTarget::FrameStats* 	m_stats;        		///< Window render statistics
	int										m_width, m_height;		///< Window size
	bool                                  	m_bIsValid;				///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called.

};

} // namespace Cing

#endif // _Window_H_
