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

#include "Camera3D.h"

// Framework
#include "framework/UserAppGlobals.h"

// Ogre includes
#include "OgreCamera.h"
#include "OgreSceneManager.h"

// Common includes
#include "common/Exception.h"
#include "common/Release.h"
#include "common/MathUtils.h"

namespace Graphics
{

// Static member initialization
const std::string   Camera3D::DEFAULT_NAME  = "DefaultCam";
const float         Camera3D::V_FOV_DEG     = 60.0f; ///< Camera's vertical FOV (in degrees)


/**
 * @internal
 * @brief Constructor. Initializes class attributes.
 */
Camera3D::Camera3D():
  m_bIsValid        ( false ),
  m_cameraSceneNode ( NULL ),
  m_pOgreCamera     ( NULL )
{
}

/**
 * @internal
 * @brief Destructor. Class release.
 */
Camera3D::~Camera3D()
{
  // Release resources
  end();
}

/**
 * @internal
 * @brief Initializes the class so it becomes valid.
 *
 * @param[in] pOgreSceneManager Ogre Scene Manager. It allows the creation of the ogre camera
 * @param[in] cameraName        Optional parameter to specify a name for the camera
 * @return true if the initialization was ok | false otherwise
 */
bool Camera3D::init( Ogre::SceneManager* pOgreSceneManager, const std::string& cameraName /*= DEFAULT_NAME*/ )
{
  // Check if the class is already initialized
  if ( isValid() )
    return true;

  // Check scene manager
  if ( !pOgreSceneManager )
    THROW_EXCEPTION( "Internal Error: NULL Scene Manager" );

  // Create camera  
  //m_pOgreCamera = pOgreSceneManager->createCamera( cameraName );
  //m_pOgreCamera->setPosition( 0, 0, 2000 );
  //m_pOgreCamera->lookAt( 0, 0, 0 );


  // Calculate the camera distance
  m_vFovRad           = Common::degToRad( V_FOV_DEG );
  m_cameraDistance    = ( static_cast< float >( Globals::height ) / 2.0f ) / tanf( m_vFovRad / 2.0f );

  // Calculate the aspect ratio
  m_aspectRatio   = static_cast< float >( Globals::width ) / static_cast< float >( Globals::height );

  // Camera pos and look at
  m_cameraPos.x = static_cast< float >( Globals::width ) / 2.0f;
  m_cameraPos.y = static_cast< float >( Globals::height ) / 2.0f;
  m_cameraPos.z = m_cameraDistance;

  m_cameraLookAt.x = static_cast< float >( Globals::width ) / 2.0f;
  m_cameraLookAt.y = static_cast< float >( Globals::height ) / 2.0f;
  m_cameraLookAt.z = 0;

  // Up vector
  m_cameraUpVector.x = 0.0f;
  m_cameraUpVector.y = -1.0f;
  m_cameraUpVector.z = 0.0f;

  // Add camera to the scene manager
  m_pOgreCamera = pOgreSceneManager->createCamera( cameraName );
  
  //TODO -> invertir vector up
  //Vector3 up = m_pOgreCamera->getUp();
  //m_pOgreCamera->setFixedYawAxis( false );
  //m_pOgreCamera->rotate( Vector3( 0, 0, 1 ), Ogre::Radian( Ogre::Degree( 180 ) ) );
  //up = m_pOgreCamera->getUp();
  ////m_pOgreCamera->setUpVector( m_cameraUpVector );

  // Set camera properties
  //m_pOgreCamera->setPosition( m_cameraPos );
  //m_pOgreCamera->lookAt( m_cameraLookAt );
  m_pOgreCamera->setFOVy( Ogre::Radian( m_vFovRad ) );
  m_pOgreCamera->setAspectRatio( m_aspectRatio );

  // Create the camera scene node
  m_cameraSceneNode = pOgreSceneManager->getRootSceneNode()->createChildSceneNode();
  m_cameraSceneNode->setPosition( m_cameraPos );
  m_cameraSceneNode->lookAt( m_cameraLookAt, Ogre::Node::TS_WORLD );
  m_cameraSceneNode->attachObject( m_pOgreCamera );

	// The class is now initialized
	m_bIsValid = true;

	return true;
}

/**
 * @internal
 * @brief Releases the class resources. 
 * After this method is called the class is not valid anymore.
 */
void Camera3D::end()
{
  // Check if the class is already released
  if ( !isValid() )
    return;

  // Delete resources
  Common::Release( m_pOgreCamera );

	// The class is not valid anymore
	m_bIsValid = false;
}

/**
 * @internal
 * @brief Updates the class state
 */
void Camera3D::update()
{

}

/**
 * @internal
 * TODO
 */
void Camera3D::moveRelative( const Vector3& move )
{
  m_pOgreCamera->moveRelative( move );
}

} // namespace Graphics