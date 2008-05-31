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

#ifndef _TexturedQuad_H_
#define _TexturedQuad_H_

#include "GraphicsPrereqs.h"

// Ogre
#include "OgreTexture.h"

namespace Graphics
{

/**
 * @internal
 * Represents a quad (two triangles) with a texture (updatable dynamically)
 */
class TexturedQuad
{
public:

	// Constructor / Destructor
	 TexturedQuad();
	~TexturedQuad();

	// Init / Release 
	bool  init            ( unsigned int textureWidth, unsigned int textureHeight, int nChannels, bool render2D = false );
	void  end             ();

	// Set methods
	void  setPosition     ( float x, float y, float z = 0.0f );
	void  setVisible      ( bool visible );

	// Texture update
	void  updateTexture   ( unsigned char* textureData, unsigned int width, unsigned int height, unsigned short channels );
	void  updateTexture   ( char* textureData, unsigned int width, unsigned int height, unsigned short channels ) { updateTexture( reinterpret_cast< unsigned char* >( textureData ), width, height, channels ); }

	// Query methods
	bool  isValid         () const { return m_bIsValid; }


private:

  // Private methods
  void generateUniqueNames  ();

  // Constant / static attributes
  static const std::string  MANUAL_OBJECT_NAME;     ///< Name of the manual object (which is the quad)
  static const std::string  MATERIAL_NAME;          ///< Name of the material where the texture will be placed
  static const std::string  TEXTURE_NAME;           ///< Name of the texture
  static long               m_quadCounter;          ///< Used to generate unique names for the quad materials, textures and ogre manual objects

	// Attributes
  Ogre::TexturePtr          m_ogreTexture;          ///< Ogre texture (to render the quad with it)  
  Ogre::SceneNode*          m_quadSceneNode;        ///< Quad scene node inside the scene (used to modify the scale, orientation...etc)
  unsigned int              m_width, m_height;      ///< Width and height of the texture
  unsigned int              m_nChannels;            ///< Number of channels of the texture (RGB -> 3, RGBA -> 4 )
  std::string               m_ogreManualObjectName; ///< Unique object name
  std::string               m_ogreTextureName;      ///< Unique texture name
  std::string               m_ogreMaterialName;     ///< Unique material name
  bool                      m_render2D;             ///< If true the quad is rendered in 2d, over the 3d scene
	bool                      m_bIsValid;	            ///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called.

};

} // namespace Graphics

#endif // _TexturedQuad_H_
