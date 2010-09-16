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

#ifndef _GraphicsManager_H_
#define _GraphicsManager_H_

#include "GraphicsPrereqs.h"
#include "Window.h"
#include "Camera3D.h"
#include "DebugOverlay.h"
#include "TextArea.h"
#include "CameraController.h"
#include "OgreManualObject.h"
#include "Transform.h"
#include "Style.h"
#include "Shape.h"
#include "Image.h"

// OpenCv includes
#include "OpenCV/cxtypes.h"
#include "OpenCV/cxcore.h"
#include "OpenCV/highgui.h"

//TEMP
#include "input/InputPrereqs.h"

// Common
#include "common/Singleton.h"
#include <vector>
#include <stack>
#include "common/eString.h"

namespace Cing
{
	/**
	* @internal
	* Manages the communication with the Graphics engine (OGRE)
	*/
	class GraphicsManager: public SingletonStatic< GraphicsManager >
	{

	public:

		// Required for singleton
		friend class SingletonStatic< GraphicsManager >;

		// Destructor
		virtual ~GraphicsManager														();

		// Init / Release / Draw
		bool						init                      ();
		void						end                       ();
		void						draw                      ();

		// App window setup
		void						setup						( int windowWidth, int windowHeight, GraphicMode mode = OPENGL );
		void						fullscreen					()		{ m_fullscreen = true; }
		void						FSAA						( int fsaa )		{ m_fsaa = fsaa; }

		// Query methods
		bool						isValid                   () const	{ return m_bIsValid; }
		const Window&				getMainWindow             () const	{ return m_mainWindow; }
		Window&						getMainWindow             ()		{ return m_mainWindow; }
		Camera3D&					getActiveCamera           ()		{ return m_activeCamera; }
		const Ogre::SceneManager&	getSceneManager           () const	{ return *m_pSceneManager; }
		Ogre::SceneManager&			getSceneManager           ()		{ return *m_pSceneManager; }

		// TODO: Experimental
		void setSceneManager           ( Ogre::SceneManager* sm )
		{
			m_pSceneManager =  sm;
		};

		Ogre::SceneManager*			getSceneManagerPtr			()       { return m_pSceneManager; }
		CameraController&			getDefaultCameraController	()		 { return m_defaultCamController; }

		// Common capabilities checking
		bool                      	hasVertexProgramsSupport	() const;
		bool                      	hasFragmentProgramsSupport	() const;
		bool                      	hasBumpMappingSupport		() const;

		// Rendering related methods
		void						setRenderMode				( RenderMode mode );

		// Color related methods
		void						setFillColor				(  const Color& color );
		void						setStrokeColor				(  const Color& color );
		const Color&				getFillColor				() const { return 	m_styles.front().m_fillColor; }
		const Color&				getStrokeColor				() const { return 	m_styles.front().m_strokeColor; }

		void						setBackgroundColor ( const Color& color );

		// Appearance drawing methods
		void 						setStrokeWeight				(  int weight );
		int  						getStrokeWeight				() { return m_styles.front().m_strokeWeight; }

		void 						noFill						() { m_fill   = false; }
		void 						noStroke					() { m_stroke = false; }
		void 						noSmooth					() { m_smooth = false; }

		bool						getFill						() const { return m_fill; }
		bool						getStroke					() const { return m_stroke; }
		bool						getSmooth 					() const { return m_smooth; }

		void 						smooth						() { m_smooth = true; }

		int							getRectMode					() const { return m_rectMode; }
		void             			setRectMode					(  int	mode );

		int							getEllipseMode				() const { return m_ellipseMode; }
		void             			setEllipseMode				(  int	mode );

		const CvFont&				getCvFont					() const { return  m_cvFont; }

		// Debug methods
		void						showFps						( bool show );

		// Camera control
		void						useDefault3DCameraControl	( bool useDefault );

		// Drawing images control (Temp)
		void						addDrawableImage			( TexturedQuad* img );
		void						removeDrawableImage			( TexturedQuad* img );

		// Stacks
		void						clearStyleStack();
		void						clearMatrixStack();

		//Save frames
		void save( const String& name )
		{
			m_saveFrame = true;
			m_frameName = name;
		};

		// Import Collada
		//bool loadCollada( const String& fileName );
		// Coordinate systems
		bool isProcessingMode() { return ( m_coordSystem == PROCESSING );	}	
		void applyCoordinateSystemTransform( const GraphicsType& coordSystem );

		// 2D Canvas
		Image						m_canvas;

		// 2D / 3D Transforms
		std::stack < Transform >	m_transforms; ///< Stack to store transform objects
		std::deque < Style >		m_styles;     ///< Deque to store style properties ( fill color, stroke weight, etc.)

	private:

		// private constructor to ensure singleton
		GraphicsManager      ();

		//TODO: Clean up
		Ogre::Light*				pLight;
		Ogre::SceneNode*			pLightSceneNode;

		// Attributes

		// Core
		Ogre::SceneManager*			m_pSceneManager;  ///< Main scene manager

		// Camera controller
		CameraController			m_defaultCamController;


		Window						m_mainWindow;     ///< Main application window
		Camera3D					m_activeCamera;   ///< Active camera

		// Graphics system setup
		int                        	m_defaultWindowWidth;
		int                        	m_defaultWindowHeight;
		GraphicMode                	m_defaultGraphicMode;
		bool                       	m_setupCalled;        ///< True when the setup method has been already called
		bool                       	m_fullscreen;         ///< If true, the applcation will run in full screen mode
		int							m_fsaa;				/// < Antialiasing value (0..16). Depends on HW support

		// Misc
		DebugOverlay				m_debugOverlay;   ///< Debug overlay used to show debug information
		//MovableText				m_systemFont;    ///< To print system info to screen

		// Styling properties
		// TODO: Eliminates this. Change styles que to stack
		int							m_rectMode;					///< Parameters input mode to draw rectangles
		int							m_ellipseMode;			///< Parameters input mode to draw ellipses
		bool						m_fill;
		bool						m_stroke;
		bool						m_smooth;

		CvFont						m_cvFont;					///< Font used to draw text on images

		// To manage visibility of loaded images
		// TODO optimize this
		std::list< TexturedQuad* >	m_drawableImagesQueue; ///< Images that are being drawn by the user ar maked as not visible every frame
		// to if the user does not call the draw one frame the image is not drawn

		//to allow screen capture and effects
		Ogre::TexturePtr			m_RttTexture;
		bool						m_saveFrame;
		String						m_frameName;

	    GraphicsType				m_coordSystem;
		bool						m_showFps;				///< Indicates whether the frames per second should be shown or not
		bool						m_bIsValid;	      ///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called
	};

} // namespace Cing

#endif // _GraphicsManager_H_
