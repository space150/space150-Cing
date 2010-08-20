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

#include "TexturedQuad.h"

#include <limits.h>

// Graphics
#include "GraphicsManager.h"

// Ogre
#include "Ogre3d/include/OgrePixelFormat.h"
#include "Ogre3d/include/OgreImage.h"
#include "Ogre3d/include/OgreSceneManager.h"
#include "Ogre3d/include/OgreManualObject.h"
#include "Ogre3d/include/OgreRenderOperation.h"
#include "Ogre3d/include/OgreHardwarePixelBuffer.h"
#include "Ogre3d/include/OgreTextureManager.h"
#include "Ogre3d/include/OgreTexture.h"
#include "Ogre3d/include/OgreMaterial.h"
#include "Ogre3d/include/OgreMaterialManager.h"

// Framework
#include "framework/UserAppGlobals.h"
#include "framework/Application.h"

// Common
#include "common/Exception.h"
#include "common/MathUtils.h"
#include "common/LogManager.h"

namespace Cing
{

// Static member initialization
const std::string  TexturedQuad::MANUAL_OBJECT_NAME  = "QuadMesh";
const std::string  TexturedQuad::MATERIAL_NAME       = "QuadMaterial";
const std::string  TexturedQuad::TEXTURE_NAME        = "QuadTexture";
long               TexturedQuad::m_quadCounter       = 0;


/**
 * @internal
 * @brief Constructor. Initializes class attributes.
 */
TexturedQuad::TexturedQuad():
  m_quadSceneNode ( NULL  ),
	m_visible				( true	),
	m_quad					( NULL  ),
	m_2dXPos				( -INT_MAX ),
	m_2dYPos				( -INT_MAX ),
	m_xScale				( 1.0f ),
	m_yScale				( 1.0f ),
	m_zScale				( 1.0f ),
	m_bIsValid      ( false )
{
}

/**
 * @internal
 * @brief Destructor. Class release.
 */
TexturedQuad::~TexturedQuad()
{
  // Release resources
  end();
}

/**
 * @internal
 * @brief Initializes the class so it becomes valid.
 *
 * @param[in] textureWidth  Width of the texture to show on the quad
 * @param[in] textureHeight Height of the texture to show on the quad
 * @param[in] format  Format of the image. RGB for color images (red, green and blue), ARGB for color plus alpha channel for transparency
 * GRAYSCALE for greyscale images, this is, black & white
 * @param[in] renderTarget if true, this texture will be used as render target (to render a scene as view from a camera)
 * @return true if the initialization was ok | false otherwise
 */
bool TexturedQuad::init( int textureWidth, int textureHeight, GraphicsType format, bool renderTarget /*= false*/ )
{
  // Check if the class is already initialized
  if ( isValid() )
    return true;

  // Check application correctly initialized (could not be if the user didn't calle size function)
  Application::getSingleton().checkSubsystemsInit();

	// Get power of 2 texture size
	 //m_textWidthP2 = firstPO2From( textureWidth );
	//m_textHeightP2 = firstPO2From( textureHeight );
	m_textWidthP2 = textureWidth;
	m_textHeightP2 = textureHeight;

  // Store the texture data
  m_textWidth     = textureWidth;
  m_textHeight    = textureHeight;
	m_format		= format;
  m_render2D  = false;

  // Generate unique names for texture, material and ogre manual object
  generateUniqueNames();
	
  // Texture Usage
  Ogre::TextureUsage usage = renderTarget? Ogre::TU_RENDERTARGET: Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE;

  // Create the texture for the quad

  // If it is a render target FSAA (anti-aliasing can be set)
  if ( usage == Ogre::TU_RENDERTARGET )
  {
	  m_ogreTexture = Ogre::TextureManager::getSingleton().createManual(m_ogreTextureName,
																		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
																		Ogre::TEX_TYPE_2D,				// type
																		(Ogre::uint)m_textWidthP2,		// width
																		(Ogre::uint)m_textHeightP2,		// height
																		0,								// number of mipmaps
																		(Ogre::PixelFormat)format,		// pixel format
																		usage,							// usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for textures updated very often (e.g. each frame)
 																		0,
																		false,
																		0);	// FSAA value
  }
  else
  {
	  m_ogreTexture = Ogre::TextureManager::getSingleton().createManual(m_ogreTextureName,
																		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
																		Ogre::TEX_TYPE_2D,				// type
																		(Ogre::uint)m_textWidthP2,		// width
																		(Ogre::uint)m_textHeightP2,		// height
																		0,								// number of mipmaps
																		(Ogre::PixelFormat)format,		// pixel format
																		usage );						// usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
																										// textures updated very often (e.g. each frame)
  }

  // Create a material for the quad
  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create( m_ogreMaterialName,
                                                                             Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  // Assign texture to material and set materia properties
  material->getTechnique(0)->getPass(0)->createTextureUnitState( m_ogreTextureName );
  material->getTechnique(0)->getPass(0)->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );
  material->getTechnique(0)->getPass(0)->setLightingEnabled( false );
  material->getTechnique(0)->getPass(0)->setDepthWriteEnabled( true );
  material->getTechnique(0)->getPass(0)->setDepthCheckEnabled( true );
  material->getTechnique(0)->getPass(0)->setCullingMode( Ogre::CULL_NONE );
  material->getTechnique(0)->getPass(0)->setAlphaRejectSettings( Ogre::CMPF_GREATER_EQUAL, 1 );

 
	/*

	TODO: We need an easy way to change images transparency!!
	//Modified by Jorge 25/feb/2009

	//material->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
  //material->getTechnique(0)->getPass(0)->setSceneBlending( Ogre::SBT_TRANSPARENT_COLOUR );
	//material->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(1,1,0.5,0.5));


  static_cast<MaterialPtr>( MaterialManager::getSingleton( ).getByName( "Overlay/TwoTextures" ) )
	->getTechnique(0)->getPass(0)->getTextureUnitState("MixTexture")->
	setColourOperationEx(LBX_BLEND_MANUAL, LBS_TEXTURE, LBS_CURRENT, 1,1, add);
  */

  // Create the manual object (is used to define geometry on the fly)
  Ogre::SceneManager& sceneManager = GraphicsManager::getSingleton().getSceneManager();
	m_quad = sceneManager.createManualObject( m_ogreManualObjectName );

  // Generate the geometry of the quad
	m_quad->begin( m_ogreMaterialName );

    // m_quad positions and texture coordinates
    //m_quad->position( 0.0, 0.0, 0.0);  m_quad->textureCoord( 0, m_textHeight / m_textHeightP2 );
    //m_quad->position( 1.0, 0.0, 0.0);  m_quad->textureCoord( m_textWidth / m_textWidthP2, m_textHeight / m_textHeightP2 );
    //m_quad->position( 1.0, 1.0, 0.0);  m_quad->textureCoord( m_textWidth / m_textWidthP2, 0 );
    //m_quad->position( 0.0, 1.0, 0.0);  m_quad->textureCoord( 0, 0 );

	// Center mode (Julio -> TODO)
    m_quad->position( -0.5, -0.5, 0.0);  m_quad->textureCoord( 0, m_textHeight / m_textHeightP2 );
    m_quad->position( 0.5, -0.5, 0.0);  m_quad->textureCoord( m_textWidth / m_textWidthP2, m_textHeight / m_textHeightP2 );
    m_quad->position( 0.5, 0.5, 0.0);  m_quad->textureCoord( m_textWidth / m_textWidthP2, 0 );
    m_quad->position( -0.5, 0.5, 0.0);  m_quad->textureCoord( 0, 0 );

	
	// m_quad indexes (two triangles)
		m_quad->triangle( 0, 1, 2 );
		m_quad->triangle( 0, 2, 3 );

  // Finish defining geometry
  m_quad->end();

  // Add to the scene
  m_quadSceneNode = sceneManager.getRootSceneNode()->createChildSceneNode();
  m_quadSceneNode->attachObject( m_quad );

	// Register in graphics manager so that it is marked as invisible every frame
	GraphicsManager::getSingleton().addDrawableImage( this );

  // The class is now initialized
	m_bIsValid = true;

	return true;
}
/**
 * @brief Change ink drawing mode
 */
void  TexturedQuad::setAdditiveMode ( bool value )
{

	if ( value )
	{
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(m_ogreMaterialName);
		material->getTechnique(0)->getPass(0)->setSceneBlending( Ogre::SBT_ADD);
	}
	else
	{
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(m_ogreMaterialName);
		material->getTechnique(0)->getPass(0)->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );
	}

};

/**
 * @internal
 * @brief Releases the class resources.
 * After this method is called the class is not valid anymore.
 */
void TexturedQuad::end()
{
  // Check if the class is already released
  if ( !isValid() )
    return;

  // Destroy quad quad object
  Ogre::SceneManager* sceneManager = GraphicsManager::getSingleton().getSceneManagerPtr();
	if ( sceneManager )
	{
		// Destroy the manual object
		sceneManager->destroyManualObject( m_ogreManualObjectName );

		// Destroy material
		Ogre::MaterialManager::getSingleton().remove( m_ogreMaterialName );

		// Destroy texture
		Ogre::TextureManager::getSingleton().remove( m_ogreTextureName );

		// Unregister in graphics manager so that it is marked as invisible every frame
		GraphicsManager::getSingleton().removeDrawableImage( this );
	}

	// The class is not valid anymore
	m_bIsValid = false;
}

/**
 * @brief Sets the position in the scene of the textured quad
 * @param[in] x x coordinate of the new position
 * @param[in] y y coordinate of the new position
 * @param[in] z z coordinate of the new position
 */
void TexturedQuad::setPosition( float x, float y, float z )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to set position in an invalid textured quad" );
		return;
	}

  m_quadSceneNode->setPosition( x, y, z );
}

/**
 * @brief Sets the position in the scene of the textured quad (in 2 dimensions -> in screen coordinates)
 * @param[in] x x coordinate of the new position
 * @param[in] y y coordinate of the new position
 */
void TexturedQuad::setPosition2d( float x, float y )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to set position in an invalid textured quad" );
		return;
	}

	// Simulate upper left corner to be 0,0 (although in ogre the 0,0 is the lower left corner
	// That is why the y screen coordinate is inverted
	m_2dXPos = (x / (float)width) * 2.0f - 1;
	m_2dYPos = -((y / (float)height) * 2.0f - 1);
	m_quadSceneNode->setPosition( m_2dXPos + m_2dWidth/2 , m_2dYPos - m_2dHeight/2, 0 );
}

/**
 * @brief Sets the scale of the object
 *
 * @param[in] xScale scale in the x axis
 * @param[in] xScale scale in the y axis
 * @param[in] xScale scale in the z axis
 */
void TexturedQuad::setScale( float xScale, float yScale, float zScale /*= 1.0f*/ )
{
	m_xScale = xScale;
	m_yScale = yScale;
	m_zScale = zScale;
	m_quadSceneNode->setScale( xScale, yScale, zScale );
}


/**
 * @brief Sets the scale of the object in 2d, this is in screen coordinates
 *
 * @param[in] xScale scale in the x axis
 * @param[in] xScale scale in the y axis
 */
void TexturedQuad::setScale2d( float xScale, float yScale )
{
	m_2dWidth = (xScale / (float)width) * 2.0f;
	m_2dHeight = (yScale / (float)height) * 2.0f;
	m_quadSceneNode->setScale( m_2dWidth, m_2dHeight, 1 );
}

/**
 * @brief Sets the rotation of the quad (in relation to its parents)
 *
 * @param[in] axis	rotation axis
 * @param[in] angle rotation angle (degrees)
 */
void TexturedQuad::setOrientation( const Vector& axis, float angle )
{
	Quaternion q( Ogre::Radian( Ogre::Degree( angle ) ), axis );
	m_quadSceneNode->setOrientation( q );
}

/**
 * @brief adds the rotation of the quad (in relation to its parents)
 *
 * @param[in] axis	rotation axis
 * @param[in] angle rotation angle (degrees)
 */
void TexturedQuad::rotate( const Vector& axis, float angle )
{
	Quaternion q( Ogre::Radian( Ogre::Degree( angle ) ), axis );
	m_quadSceneNode->rotate( q );
}

/**
 * @internal
 * Sets the texture quad as visible/invisible
 * @param[in] visible if true the quad will be visible
 */
void TexturedQuad::setVisible( bool visible )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to set position in an invalid textured quad" );
		return;
	}

	m_visible = visible;
  m_quadSceneNode->setVisible( visible );
}

/**
 * @brief Draws the texture quad in three dimensions, with the texture size
 *
 * @param x X coordinate where it will be drawn
 * @param y Y coordinate where it will be drawn
 * @param z Z coordinate where it will be drawn
 */
void TexturedQuad::draw( float x, float y, float z )
{
	draw( x, y, z, m_textWidth * m_xScale, m_textHeight * m_yScale );
}

/**
 * @brief Draws the texture quad in three dimensions, with a specific size
 *
 * @param x 			X coordinate where it will be drawn
 * @param y 			Y coordinate where it will be drawn
 * @param z 			Z coordinate where it will be drawn
 * @param width		Width of the quad that will be rendered
 * @param height	Height of the quad that will be rendered
 */
void TexturedQuad::draw( float x, float y, float z, float width, float height )
{
	if ( !isValid() )
	{
		LOG_ERROR_NTIMES( 1, "Trying to draw a textured quad not initialized" );
		return;
	}

	// If the object is set to render in 2d -> set it to render in 3d
	if ( m_render2D )
		set3dRendering();
    
	// Finally, set coordinates in the selected system
	if ( GraphicsManager::getSingleton().isProcessingMode() )
	{
		m_quadSceneNode->setPosition( x + width/2, y + height/2, z );
		m_quadSceneNode->setScale( width, -height, 1 );
	}else{
		m_quadSceneNode->setPosition( x, y, z );
		m_quadSceneNode->setScale( width, height, 1 );
	}

	m_quadSceneNode->setVisible( true );
}

/**
* @brief Draws the texture quad in three dimensions, specifying four points
* order: top-left, top-right, bottom-right, bottom-left (anti-cloclwise)
*
* @param xPos x1
* @param yPos y1
* @param zPos z1
* @param xPos x2
* @param yPos y2
* @param zPos z2
* @param xPos x3
* @param yPos y3
* @param zPos z3
* @param xPos x4
* @param yPos y4
* @param zPos z4
*/
void TexturedQuad::draw( float x1, float y1, float z1,
												 float x2, float y2, float z2,
												 float x3, float y3, float z3,
												 float x4, float y4, float z4)
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to draw a textured quad not initialized" );
		return;
	}

	// If the object is set to render in 2d -> set it to render in 3d
	if ( m_render2D )
		set3dRendering();

	// Generate the geometry of the quad
	m_quad->beginUpdate( 0 );

	// Coordinate systems: in normal system invert y coordinates (y increasis in top direction)
	if ( GraphicsManager::getSingleton().isProcessingMode() == false )
	{
		y1 = height - y1;
		y2 = height - y2;
		y3 = height - y3;
		y4 = height - y4;
	}

	// m_quad positions and texture coordinates
	// TODO revisar esto...por qu� hay q voltear las coordenadas uv?
	m_quad->position( x1, y1, z1 );  m_quad->textureCoord( 0, 0 );
	m_quad->position( x2, y2, z2 );  m_quad->textureCoord( 1, 0 );
	m_quad->position( x3, y3, z3 );  m_quad->textureCoord( 1, 1 );
	m_quad->position( x4, y4, z4 );  m_quad->textureCoord( 0, 1 );

	// m_quad indexes (two triangles)
	m_quad->triangle( 0, 2, 1 );
	m_quad->triangle( 0, 3, 2 );

	// Finish defining geometry
	m_quad->end();

	// Set properties of the quad, and set visible -> it will be rendered in the next render
	m_quadSceneNode->setVisible( true );
}

/**
 * @brief Draws the texture quad in two dimensions, with a specific size
 *
 * @param x X coordinate where it will be drawn <b>in screen coordinates</b>
 * @param y Y coordinate where it will be drawn <b>in screen coordinates</b>
 * @param width		Width of the quad that will be rendered <b>in screen coordinates</b>
 * @param height	Height of the quad that will be rendered <b>in screen coordinates</b>
 */
void TexturedQuad::draw2d( float x, float y, float imgWidth, float imgHeight )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to draw a textured quad not initialized" );
		return;
	}

	// If the object was set to render in 3d -> set it to render in 2d
	if ( !m_render2D )
		set2dRendering();

	// Set properties of the quad, and set visible -> it will be rendered in the next render
	if ( GraphicsManager::getSingleton().isProcessingMode() )
	{
		// Set properties of the quad, and set visible -> it will be rendered in the next render
		setScale2d( imgWidth, -imgHeight );
		setPosition2d( x, height-y );
	}else{
		// Set properties of the quad, and set visible -> it will be rendered in the next render
		setScale2d( imgWidth, imgHeight );
		setPosition2d( x, y );
	}
	m_quadSceneNode->setVisible( true );
}

/**
 * @brief Draws the texture quad in two dimensions, with the texture size
 *
 * @param x X coordinate where it will be drawn <b>in screen coordinates</b>
 * @param y Y coordinate where it will be drawn <b>in screen coordinates</b>
 */
void TexturedQuad::draw2d( float x, float y )
{
	draw2d( x, y, m_textWidth * m_xScale, m_textHeight * m_yScale );
}

/**
 * @brief Draws the texture quad in two dimensions, specifying the four corner points
 * order: top-left, top-right, bottom-right, bottom-left (anti-cloclwise)
 */
void TexturedQuad::draw2d( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4 )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to draw a textured quad not initialized" );
		return;
	}

	// If the object was set to render in 3d -> set it to render in 2d
	if ( !m_render2D )
		set2dRendering();

	// Generate the geometry of the quad
	m_quad->beginUpdate( 0 );

	// Coordinate systems: in normal system invert y coordinates (y increasis in top direction)
	if ( GraphicsManager::getSingleton().isProcessingMode() )
	{
		y1 = height - y1;
		y2 = height - y2;
		y3 = height - y3;
		y4 = height - y4;
	}

	// m_quad positions and texture coordinates
	// TODO revisar esto...por qu� hay q voltear las coordenadas uv?
	m_quad->position( (x1 / (float)width) * 2.0f - 1, -((y1 / (float)height) * 2.0f - 1), 0 );  m_quad->textureCoord( 0, 0 );
	m_quad->position( (x2 / (float)width) * 2.0f - 1, -((y2 / (float)height) * 2.0f - 1), 0 );  m_quad->textureCoord( 1, 0 );
	m_quad->position( (x3 / (float)width) * 2.0f - 1, -((y3 / (float)height) * 2.0f - 1), 0 );  m_quad->textureCoord( 1, 1 );
	m_quad->position( (x4 / (float)width) * 2.0f - 1, -((y4 / (float)height) * 2.0f - 1), 0 );  m_quad->textureCoord( 0, 1 );

	// m_quad indexes (two triangles)
	m_quad->triangle( 0, 2, 1 );
	m_quad->triangle( 0, 3, 2 );

	// Finish defining geometry
	m_quad->end();

	// Set properties of the quad, and set visible -> it will be rendered in the next render
	m_quadSceneNode->setVisible( true );
}

// TODO: NOTE --> deprecated --> update
void TexturedQuad::drawBackground( float x, float y )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to draw a textured quad not initialized" );
		return;
	}
	if ( GraphicsManager::getSingleton().isProcessingMode() )
	{
		// Set properties of the quad, and set visible -> it will be rendered in the next render
		setScale2d( m_textWidth, -m_textHeight );
		setPosition2d( x, height-y );
	}else{
		// Set properties of the quad, and set visible -> it will be rendered in the next render
		setScale2d( m_textWidth, m_textHeight );
		setPosition2d( x, height-y );
	}

	m_quadSceneNode->setVisible( true );
}

void TexturedQuad::setTransparency( float alpha )
{
	// Has alpha channel?
	if ( Ogre::PixelUtil::getNumElemBytes( (Ogre::PixelFormat)m_format ) )
	{
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName( m_ogreMaterialName );
		material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation( Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, alpha );
	}
	else
	{
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName( m_ogreMaterialName );
		material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, alpha);
	}
}

void TexturedQuad::setRenderQueue( Ogre::RenderQueueGroupID group )
{
	if ( isValid() )
		m_quad->setRenderQueueGroup( group );
}


/**
* @brief Draws the texture quad in two dimensions as a background
* 
* @param x X coordinate where it will be drawn <b>in screen coordinates</b>
* @param y Y coordinate where it will be drawn <b>in screen coordinates</b>
*/
void TexturedQuad::drawBackground( float x, float y, float imgWidth, float imgHeight )
{
	if ( !isValid() )
	{
		LOG_ERROR( "Trying to draw a textured quad not initialized" );
		return;
	}

	// Properties to be rendered in 2d
	m_quad->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND + 1);
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(m_ogreMaterialName);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );

	m_quad->setUseIdentityProjection(true);
	m_quad->setUseIdentityView(true);
	Ogre::AxisAlignedBox aabb;
	aabb.setInfinite();
	m_quad->setBoundingBox(aabb);

	// mark the object as 2d rendering
	m_render2D = true;

	// Set properties of the quad, and set visible -> it will be rendered in the next render
	if ( GraphicsManager::getSingleton().isProcessingMode() )
	{
		// Set properties of the quad, and set visible -> it will be rendered in the next render
		setScale2d( imgWidth, imgHeight);
		setPosition2d( x, height - y - imgHeight );
	}else{
		// Set properties of the quad, and set visible -> it will be rendered in the next render
		setScale2d( imgWidth, imgHeight );
		setPosition2d( x, y );
	}

	m_quadSceneNode->setVisible( true );
}

/**
 * @brief Updates the texture pixel information from an Image
 *
 * @param	img Image containing the pixel data to upload to the texture
 */
void TexturedQuad::updateTexture( const Ogre::Image& img )
{
	// Check if the object is valid
	if ( !isValid() )
		THROW_EXCEPTION( "Trying to upload data texture to a not initializad TextureQuad object" );

	// Update texture
	m_ogreTexture->getBuffer( 0, 0 )->blitFromMemory( img.getPixelBox( 0, 0 ) );
}

/**
 * @internal
 * @brief Updates the texture pixel data
 * @param[in] textureData New pixel data to upload to the texture
 * @param[in] width       Width of the new pixel data
 * @param[in] height      Height of the new pixel data
 * @param[in] format  Format of the image. RGB for color images (red, green and blue), ARGB for color plus alpha channel for transparency
 * GRAYSCALE for grayscale images, this is, black & white
 */
void TexturedQuad::updateTexture( unsigned char* textureData, int width, int height, GraphicsType format )
{
	// Check if the object is valid
	if ( !isValid() )
		THROW_EXCEPTION( "Trying to upload data texture to a not initializad TextureQuad object" );

  // Check resolution
  if ( ( width > m_textWidth ) || ( height > m_textHeight ) )
    THROW_EXCEPTION( "Trying to update texture with too big image data" );

	// Check format
	if ( m_format != format )
		THROW_EXCEPTION( "Trying to update texture with different format" );

	// Update texture
	//m_ogreTexture->getBuffer( 0, 0 )->blitFromMemory( Ogre::PixelBox( width, height, 1, (Ogre::PixelFormat)format, textureData ) );
	Ogre::PixelBox newData( width, height, 1, (Ogre::PixelFormat)format, textureData );
	Ogre::PixelBox dest( m_textWidth, m_textHeight, 1, (Ogre::PixelFormat)format, textureData );
	m_ogreTexture->getBuffer( 0, 0 )->blitFromMemory( newData, dest );
}

/**
 * @brief Flips the texture coordinates vertically
 */
void TexturedQuad::flipVertical()
{
	// Update geometry to flip texture coordinates
	// We recreate the object instead of updating it because this is an extrange use, and for the normal
	// use is better in terms of performance to have static objects (in terms of vertex data)
	m_quad->clear();
	m_quad->begin( m_ogreMaterialName );

	// m_quad positions and texture coordinates
	m_quad->position( 0.0, 0.0, 0.0);  m_quad->textureCoord( 0, 0 );
	m_quad->position( 1.0, 0.0, 0.0);  m_quad->textureCoord( 1, 0 );
	m_quad->position( 1.0, 1.0, 0.0);  m_quad->textureCoord( 1, 1 );
	m_quad->position( 0.0, 1.0, 0.0);  m_quad->textureCoord( 0, 1 );

	// m_quad indexes (two triangles)
	m_quad->triangle( 0, 2, 1 );
	m_quad->triangle( 0, 3, 2 );
// Versi�n que estaba en carpeta cing
//	m_quad->triangle( 0, 1, 2 );
//	m_quad->triangle( 0, 2, 3 );

	// Finish updating geometry
	m_quad->end();
}


/**
 * @brief Copies the data of the received texture quad
 *
 * @param other TextureQuad to copy
 */
void TexturedQuad::operator=( const TexturedQuad& other )
{
	// Check the other texture quad is valid
	if ( !other.isValid() )
		THROW_EXCEPTION( "Trying to copy an invalid TextureQuad" );

	// Init the texture
	end(); // TODO: optimize this
	init( other.m_textWidth, other.m_textHeight, other.m_format );

	// Copy the texture
	other.m_ogreTexture->copyToTexture( m_ogreTexture );

	// Copy attributes
	const Vector& pos = other.m_quadSceneNode->getPosition();
	setPosition( pos.x, pos.y, pos.z );
	setVisible( other.m_visible );

}

/**
 * @internal
 * @brief Generates unique names for this quad's texture, material and ogre manual object
 */
void TexturedQuad::generateUniqueNames()
{
  ++m_quadCounter;
  std::ostringstream oss;

  // Manual object
  oss << MANUAL_OBJECT_NAME << m_quadCounter;
  m_ogreManualObjectName = oss.str();
  oss.str( "" );

  // Texture
  oss << TEXTURE_NAME << m_quadCounter;
  m_ogreTextureName = oss.str();
  oss.str( "" );

  // Material
  oss << MATERIAL_NAME << m_quadCounter;
  m_ogreMaterialName = oss.str();
}

/**
 * @internal
 * @brief Sets the necessary properties for the object to be rendered in 2d (in screen coordinates and size in pixels)
 */
void TexturedQuad::setbackgroundRendering()
{
	// Properties to be rendered in 2d
	m_quad->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
	m_quad->setUseIdentityProjection(true);
	m_quad->setUseIdentityView(true);
	Ogre::AxisAlignedBox aabb;
	aabb.setInfinite();
	m_quad->setBoundingBox(aabb);

	// mark the object as 2d rendering
	m_render2D = true;
}

/**
 * @internal
 * @brief Sets the necessary properties for the object to be rendered in 2d (in screen coordinates and size in pixels)
 */
void TexturedQuad::set2dRendering()
{
	// Properties to be rendered in 2d
	m_quad->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY -1);
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(m_ogreMaterialName);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled( true );
	m_quad->setUseIdentityProjection(true);
	m_quad->setUseIdentityView(true);
	Ogre::AxisAlignedBox aabb;
	aabb.setInfinite();
	m_quad->setBoundingBox(aabb);

	// mark the object as 2d rendering
	m_render2D = true;
}

/**
 * @internal
 * @brief Sets the necessary properties for the object to be rendered in 3d (in world coordinates and world units)
 */
void TexturedQuad::set3dRendering()
{
	// Set properties for 3d rendering
	m_quad->setRenderQueueGroup( Ogre::RENDER_QUEUE_MAIN );
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(m_ogreMaterialName);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled( true );
	m_quad->setUseIdentityProjection( false );
	m_quad->setUseIdentityView( false );
	m_quad->setQueryFlags( m_3dQueryFlags );

	// mark the object as 3d rendering
	m_render2D = false;
}


} // namespace Cing
