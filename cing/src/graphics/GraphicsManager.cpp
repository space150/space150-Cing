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

// Graphics includes
#include "GraphicsManager.h"
#include "Window.h"
#include "ImageResourceManager.h"
#include "TexturedQuad.h"
#include "Image.h"
#include "ShapeManager.h"
#include "FontManager.h"
#include "GraphicsUserApi.h"

// Framework
#include "framework/UserAppGlobals.h"
#include "framework/Application.h"


// Common includes
#include "common/Exception.h"
#include "common/Release.h"
#include "common/MathUtils.h"
#include "common/CommonConstants.h"
#include "common/ResourceManager.h"
#include "common/LogManager.h"

// Ogre includes
#include "Ogre3d/include/OgreRoot.h"
#include "Ogre3d/include/OgreConfigFile.h"
#include "Ogre3d/include/OgreRenderWindow.h"
#include "Ogre3d/include/OgreRenderSystem.h"
#include "Ogre3d/include/OgreBillboard.h"
#include "Ogre3d/include/OgreBillboardSet.h"
#include "Ogre3d/include/OgreStringConverter.h"
#include "Ogre3d/include/OgreStringConverter.h"
#include "Ogre3d/include/OgreHardwarePixelBuffer.h"

// TEMP
#include "Ogre3d/include/OgreTextAreaOverlayElement.h"

// GUI
//#include "gui/GUIManager.h"

// Collada
#include "OgreCollada/include/OgreCollada.h"

namespace Cing
{

	/**
	* @internal
	* @brief Constructor. Initializes class attributes.
	*/
	GraphicsManager::GraphicsManager():
m_bIsValid    ( false ),
m_showFps			( false ),
m_pSceneManager( NULL ),
m_canvas( NULL ),
m_fill( true ),
m_stroke( true ),
m_smooth( false ),
m_rectMode( CORNER ),
m_ellipseMode( CENTER ),
m_setupCalled( false ),
m_defaultWindowWidth( 640 ),
m_defaultWindowHeight( 480 ),
m_defaultGraphicMode( OPENGL ),
m_fullscreen( false ),
m_saveFrame(false)
{
}

/**
* @internal
* @brief Destructor. Class release.
*/
GraphicsManager::~GraphicsManager()
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
bool GraphicsManager::init()
{
	// Check if the class is already initialized
	if ( isValid() )
		return true;

	// Get reference to Ogre Root
	Ogre::Root& ogreRoot = Ogre::Root::getSingleton();

	// Show config dialog
	//if ( /*!ogreRoot.restoreConfig() &&*/ !ogreRoot.showConfigDialog() )
	//  return false;
	//  //THROW_EXCEPTION( "User canceled the config dialog!" );

	// Setup default values if user didn't call setup
	setup( m_defaultWindowWidth, m_defaultWindowHeight );

	// Init rendering engine and create main window
	Ogre::RenderWindow* ogreWindow = ogreRoot.initialise( true, "Cing" );
	if ( !ogreWindow )
		THROW_EXCEPTION( "Error creating application window" );

	// Create main window
	m_mainWindow.init( ogreWindow );

	// Set global window size variables
	width = m_mainWindow.getWidth();
	height = m_mainWindow.getHeight();

	// Create the scene manager
	m_pSceneManager = ogreRoot.createSceneManager( Ogre::ST_GENERIC );

	// Set the global pointer to the scene manager
	ogreSceneManager	= m_pSceneManager;

	// PreInit GUI Manager (QuickGUI requirements)
	//GUIManager::getSingleton().preInit();

	// Initialize graphics resources, parse scripts etc
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();


	// Init ImageResourceManager
	ImageResourceManager::getSingleton().init();

	// Init ShapeManager
	ShapeManager::getSingleton().init();

	// Init the Font Manager
	FontManager::getSingleton().init();

	// Init the main camera
	m_activeCamera.init( m_pSceneManager );

	// Set the global pointer to the camera
	ogreCamera	= m_activeCamera.getOgreCamera();

	// Make the camera render in the main window
	m_mainWindow.attachCameraToWindow( m_activeCamera );

	// Background color
	m_mainWindow.setBackgroundColor( Color( 200, 200, 200 ) );

	// Init the debug overlay
	// TODO
	//m_debugOverlay.init();

	// Use default camera controller
	useDefault3DCameraControl( true );

	// Init the cvFont
	cvInitFont(&m_cvFont, CV_FONT_HERSHEY_SIMPLEX, 0.6, 0.6, 0, 2);

	// Init GUI Manager
	//GUIManager::getSingleton().init();

	// Init 2dCanvas
	m_canvas = new Image(width, height, RGB);

	// Init style queue
	m_styles.push_front( Style(Color(255,255,255), Color(0,0,0), 1) );

	// Init transform stack
	m_transforms.push( Transform() );

	// Init pixels
	for (int i = 0; i < m_canvas->getWidth() * m_canvas->getHeight(); i++)
		pixels.push_back( Color( 200, 200, 200 ) );

	// Set image background color
	m_canvas->fill(Color(200));

	// Init RTT texture and setup viewport
	m_RttTexture = Ogre::TextureManager::getSingleton().createManual("RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, m_mainWindow.getWidth(), m_mainWindow.getHeight(), 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	Ogre::RenderTarget* rttTex	= m_RttTexture->getBuffer()->getRenderTarget();
	rttTex->setAutoUpdated(false);
	Ogre::Viewport* vp	= rttTex->addViewport( m_activeCamera.getOgreCamera() );
	vp->setOverlaysEnabled(true);

	// Init the default font / text
	//m_systemFont.init( width, height);
	//m_systemFont.setPos( 0.01f, 0.01f );		        // Text position, using relative co-ordinates
	//m_systemFont.setCol( Color( 100 ) );	// Text color (Red, Green, Blue, Alpha)  

	// Set default coordinate system:
	m_coordSystem = NORMAL;

	// This is to adjust 2d and 3d coordinates like in Processing:
	applyCoordinateSystemTransform(PROCESSING);

	// The class is now initialized
	m_bIsValid = true;

	return true;
}

/**
* @internal
* @brief Releases the class resources.
* After this method is called the class is not valid anymore.
*/
void GraphicsManager::end()
{
	// Check if the class is already released
	if ( !isValid() )
		return;

	// Release GUI Manager
	//GUIManager::getSingleton().end();

	// Release camera stuff
	m_defaultCamController.end();
	m_activeCamera.end();

	// Release scene manager
	Ogre::Root::getSingleton().destroySceneManager( m_pSceneManager );
	m_pSceneManager = NULL;

	// Release image resource manager
	ImageResourceManager::getSingleton().end();

	// Release the Font Manager
	FontManager::getSingleton().end();

	// Release the Shape Manager
	ShapeManager::getSingleton().end();

	// Release 2dCanvas...
	m_canvas->end();
	m_canvas = NULL;

	// The class is not valid anymore
	m_bIsValid = false;
}

/**
* @internal
* @brief Renders everything in the scene
*/
void GraphicsManager::draw()
{

	// Update the background image
	m_canvas->drawBackground(	0,
		0,
		m_mainWindow.getOgreWindow()->getViewport(0)->getActualWidth(),
		m_mainWindow.getOgreWindow()->getViewport(0)->getActualHeight());

	// Update 3d primitive drawing	( shape, lines,...)
	ShapeManager::getSingleton().update();

	// Update default camera controller
	m_defaultCamController.update();

	// Render scene
	Ogre::Root::getSingleton().renderOneFrame();

	// Update the Font Manager post render
	FontManager::getSingleton().postRender();


	// Update window
	m_mainWindow.update();

	// Get Frame stats
	const Ogre::RenderTarget::FrameStats& frameStats = m_mainWindow.getFrameStats();

	frameRate = frameStats.avgFPS;

	// Show fps
	if ( m_showFps )
	{
		std::ostringstream oss;
		oss << "FPS: " << frameStats.lastFPS;
		text( oss.str(), 10, 0 );
		//m_systemFont.setText( oss.str() );	// Text to be displayed
		//m_systemFont.show( true );
	}
	//else
	//	m_systemFont.show( false );

	// Render the viewport to texture and save to disk if required
	if ( m_saveFrame )
	{
		m_RttTexture->getBuffer()->getRenderTarget()->update();
		m_RttTexture->getBuffer()->getRenderTarget()->writeContentsToFile(ResourceManager::userDataPath + m_frameName );
		m_saveFrame = false;
	}

	// Mark all drawable images as not visible
	std::list< TexturedQuad* >::iterator it = m_drawableImagesQueue.begin();
	for (; it != m_drawableImagesQueue.end(); ++it )
		(*it)->setVisible( false );

	// Reset the "global" matrix stack
	clearMatrixStack();

}

/**
* @brief Configures the render system and window for the application.
* @note It should be called first to any other graphics related function
* @param windowWidth   width of the application's window
* @param windowHeight  height of the application's window
* @param mode          specifies the render driver to use. Default OPENGL
*/
void GraphicsManager::setup( int windowWidth, int windowHeight, GraphicMode mode )
{
	// Check if setup has already been called
	if ( m_setupCalled )
		return;

	// Get ogre root to configure it
	Ogre::Root& ogreRoot = Ogre::Root::getSingleton();

	// Name of the chosen render system
	std::string rendererName = "NO_NAME";
	if ( mode == OPENGL )
		rendererName = "OpenGL Rendering Subsystem";
	else if ( mode == DIRECTX )
		rendererName = "Direct3D9 Rendering Subsystem";
	else
		THROW_EXCEPTION( "Critital Error: Graphics Mode in size() call does not exist" );

	// Get list of render systems
	Ogre::RenderSystemList  *availableRenderers = ogreRoot.getAvailableRenderers();
	Ogre::RenderSystem      *selectedRenderSystem = NULL;
	if ( !availableRenderers || ( availableRenderers->size() == 0 ) )
		THROW_EXCEPTION( "Critical Error: No available render systems. Please re-install the application" );

	// Select the render system
	for ( size_t i = 0; i < availableRenderers->size(); ++i )
	{
		if ( availableRenderers->at( i )->getName().compare( rendererName ) == 0 )
		{
			selectedRenderSystem = availableRenderers->at( i );
			break;
		}
	}

	// Set render system to ogre
	ogreRoot.setRenderSystem( selectedRenderSystem );

	// Configure rest of the settings depending on the rendering system selected
	if ( mode == OPENGL )
	{
		// Generate the video mode string
		std::ostringstream videoMode;
		videoMode << windowWidth << " x " << windowHeight;;

		// Set render system settings
		selectedRenderSystem->setConfigOption("Full Screen", m_fullscreen? "Yes": "No" );
		if ( !m_fullscreen )
			selectedRenderSystem->setConfigOption("Video Mode", videoMode.str().c_str() );

		// Set render system settings specified by user
		// TODO: Make all options available to user
		selectedRenderSystem->setConfigOption( "Colour Depth", "32" );
		selectedRenderSystem->setConfigOption( "VSync","Yes" );

	}
	else if ( mode == DIRECTX )
	{
		// Generate the video mode string
		std::ostringstream videoMode;
		videoMode << windowWidth << " x " << windowHeight << " @ 32-bit colour";

		// Set render system settings specified by user
		selectedRenderSystem->setConfigOption("Full Screen", m_fullscreen? "Yes": "No" );
		if ( !m_fullscreen )
			selectedRenderSystem->setConfigOption("Video Mode", videoMode.str().c_str() );

		// Rest options with default values
		selectedRenderSystem->setConfigOption("VSync","Yes");

	}


	// Set up done
	m_setupCalled = true;
}

/**
* @internal
* @brief   Reset style stack
*/
void GraphicsManager::clearStyleStack()
{
	m_styles.clear();
	m_styles.push_front(Style(Color(255,255,255), Color(0,0,0), 1));
};
/**
* @internal
* @brief   Reset matrix stack (Clear and add one identity transform)
*/
void GraphicsManager::clearMatrixStack()
{
	while ( !m_transforms.empty() )
		m_transforms.pop();
	m_transforms.push(Transform());
};

/**
* @internal
* @brief   Returns true if the system supports vertex programs (vertex shaders)
* @return  true if the system supports vertex programs (vertex shaders)
*/
bool GraphicsManager::hasVertexProgramsSupport() const
{
	// Get system capabilities
	const Ogre::RenderSystemCapabilities* caps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();

	// Check vertex programs
	if ( caps->hasCapability( Ogre::RSC_VERTEX_PROGRAM ) )
		return true;

	// Not supported
	return false;
}

/**
* @internal
* @brief   Returns true if the system supports fragment programs (pixel shaders)
* @return  true if the system supports pixel programs (pixel shaders)
*/
bool GraphicsManager::hasFragmentProgramsSupport() const
{
	// Get system capabilities
	const Ogre::RenderSystemCapabilities* caps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();

	// Check fragment programs
	if ( caps->hasCapability( Ogre::RSC_FRAGMENT_PROGRAM ) )
		return true;

	// Not supported
	return false;
}


/**
* @internal
* @brief   Returns true if the system supports bump mapping.
* To support bump mapping the system need support of: vertex programs, and (fragment programs or dot3 texture
* in the fixed-function pipeline)
* @return  true if the system supports bump mapping
*/
bool GraphicsManager::hasBumpMappingSupport() const
{
	// Get system capabilities
	const Ogre::RenderSystemCapabilities* caps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();

	// Check capabilities programs
	if (  caps->hasCapability( Ogre::RSC_VERTEX_PROGRAM ) &&
		( caps->hasCapability( Ogre::RSC_FRAGMENT_PROGRAM ) || caps->hasCapability( Ogre::RSC_DOT3 ) ) )
		return true;

	// Not supported
	return false;
}

/**
* @brief Sets the render mode used to draw the objects in the scene
*
* @param mode Active render mode. Possible values: POINTS, WIREFRAME, SOLID
*/
void GraphicsManager::setRenderMode( RenderMode mode )
{
	// Check application correctly initialized (could not be if the user didn't call size function)
	Application::getSingleton().checkSubsystemsInit();

	m_activeCamera.getOgreCamera()->setPolygonMode( (Ogre::PolygonMode) mode );
}

/**
* @brief Sets the fill color used to draw all shapes rendered after this call.
*
* @param Color color to use to fill shapes
*/
void GraphicsManager::setFillColor( const Color& color )
{
	// Check application correctly initialized (could not be if the user didn't call size function)
	Application::getSingleton().checkSubsystemsInit();

	m_styles.front().m_fillColor = color;

	// We are using the emissive color to fake the fill color with lighting activated
	// TODO dejar esto bien
	//m_pSceneManager->setAmbientLight( m_styles.front().m_fillColor );

	m_fill = true;
}

/**
* @brief Sets the border color used to draw shapes after this call.
*
* @param Color color to use to draw the border of shapes
*/
void GraphicsManager::setStrokeColor( const Color& color )
{
	// Check application correctly initialized (could not be if the user didn't call size function)
	Application::getSingleton().checkSubsystemsInit();

	m_styles.front().m_strokeColor = color;

	m_stroke = true;
}

/**
* @brief Sets the border color used to draw shapes after this call.
*
* @param Color color to use to draw the border of shapes
*/
void GraphicsManager::setStrokeWeight( int weight )
{
	// Check application correctly initialized (could not be if the user didn't call size function)
	Application::getSingleton().checkSubsystemsInit();

	m_styles.front().m_strokeWeight = weight;
}

/**
* @internal
* @brief Makes the frames per second to be printed on the screen or not
*
* @param show if true the current fps wil be printed on screen, if false, it won't be printed
*/
void GraphicsManager::showFps( bool show )
{
	m_showFps = show;
}


/**
* @brief Allows to enable or disable the default 3d camera control
*
* @param useDefault If true, the default camera control will be enabled.
* If false, it will be disable, so the user will be reposible to control the 3d camera
* if it is required.
*
* @note The default camera control is composed by:
* - mouse: controls rotation
* - keyboard:
*		- arrow keys: moves forward/backward/left/right
*		- pg up/down: moves camera up and down
*		- r: restore camera rotation to initial settings
*/
void GraphicsManager::useDefault3DCameraControl( bool useDefault )
{
	// Check application correctly initialized (could not be if the user didn't call size function)
	Application::getSingleton().checkSubsystemsInit();

	// Enable controller
	if ( useDefault )
		m_defaultCamController.init( m_activeCamera );
	// Enable controller
	else
		m_defaultCamController.end();
}

/**
* @internal
* @brief Informs that an image is created (so it can be be drawn), it will be made invisible after each frame is rendered.
* This way, if the user does not call the draw method for the same image in any frame, it won't be rendered
* @note This is a bit triky, but allows to emulate software rendering (this is, the image is renderd just when the draw method
* is called), when it is really working with ogre's 3d scene manager
*
* @param img Image that is going to be rendered
*/
void GraphicsManager::addDrawableImage( TexturedQuad* img)
{
	m_drawableImagesQueue.push_back( img );
}

/**
* @internal
* @brief Informs that an image that was added as drawable image (@sa addDrawableImage) is being released
*
* @param img Image that is going to be rendered
*/
void GraphicsManager::removeDrawableImage( TexturedQuad* img)
{
	std::list< TexturedQuad* >::iterator it = m_drawableImagesQueue.begin();
	for (; it != m_drawableImagesQueue.end(); ++it )
		if ( *it == img )
		{
			m_drawableImagesQueue.erase( it );
			return;
		}
}

/**
* @brief Modifies the location from which rectangles draw
*/
void GraphicsManager::setRectMode( int	mode )
{
	m_rectMode = mode;
}

/**
* @brief Modifies the location from which ellipses draw
*/
void GraphicsManager::setEllipseMode( int	mode )
{
	m_ellipseMode = mode;
}


/**
* @internal
* @brief Modifies the background of the window
* (really the main viewport in the window)
* @param[in] color Color to set to the background
*/
void GraphicsManager::setBackgroundColor( const Color& color )
{
	// Check application correctly initialized (could not be if the user didn't call size function)
	Application::getSingleton().checkSubsystemsInit();

	if ( !isValid() )
		return;

	m_mainWindow.getOgreWindow()->getViewport(0)->setBackgroundColour( color.normalized() );

	cvSet( &m_canvas->getCVImage(), cvScalar(color.b,color.g,color.r) );
	m_canvas->setUpdateTexture(true);
}

/**
* @brief Apply coordinate system transforms to subsequent rendering and camera
*
* @param coordSystem
*/
void GraphicsManager::applyCoordinateSystemTransform( GraphicsType coordSystem )
{
	if ( coordSystem == m_coordSystem)
		return;

	m_coordSystem = coordSystem;
	switch(m_coordSystem)
	{
	case NORMAL:
		{
			// Reset camera position (and orientation?)
			m_activeCamera.getSceneNode()->setPosition( Ogre::Vector3( 0, 0, 2000.0 ) );
			// Reset applied simmetry to y-world axis
			m_pSceneManager->getRootSceneNode()->setScale(1,1,1);
		}
		break;
	case PROCESSING:
		{
			// Calculate new camera position 
			float cameraDistance    =  (height / 2.0f ) / tanf( (m_activeCamera.getOgreCamera()->getFOVy().valueRadians()) / 2.0f );

			Ogre::Vector3 camPos = Ogre::Vector3( width/2.0, height/2.0, cameraDistance );
			// Set the camera position

			m_activeCamera.getSceneNode()->setPosition( camPos );
			m_activeCamera.getSceneNode()->lookAt( Ogre::Vector3(width/2.0, height/2.0, 0), Ogre::Node::TS_WORLD );

			// Apply simmetry to y-world axis
			m_pSceneManager->getRootSceneNode()->setScale(1,-1,1);
		}	
		break;
	default:
		break;
	}

}

} // namespace Cing