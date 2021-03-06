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

#ifndef _BaseCameraInput_H_
#define _BaseCameraInput_H_

// Precompiled headers
#include "Cing-Precompiled.h"


#include "CameraInputPrereqs.h"
#include "common/MathUtils.h"

// OpenCV
#include "OpenCV/cxtypes.h"

// Graphics
#include "graphics/Image.h"
#include "graphics/GraphicsTypes.h"

// Ogre
#include "OgreTimer.h"

namespace Cing
{

	/**
	* @internal
	* Base class for all the camera input (capture) devices.
	*/
	class BaseCameraInput
	{
	public:

		// Constructor / Destructor
		BaseCameraInput ();
		virtual ~BaseCameraInput();

		// Init / Release / Update (to be implemented in subclasses)
		virtual bool    init            ( int deviceId = 0, int width = 320, int height = 240, int fps = 25, GraphicsType format = RGB, bool multithreaded = true );
		virtual void    end             ();
		virtual void    update          () = 0;

		// Misc
		void			draw		();
		void			showFps	( bool showFps ) { m_showFps = showFps; }

		// Query methods
		bool            isValid			() const { return m_bIsValid;   }
		bool            haveNewFrame    () const { return m_newFrame;   }
		int             getWidth        () const { return m_width;      }
		int             getHeight       () const { return m_height;     }
		int             getFPS          () const { return m_fps;        }
		int             getNumChannels  () const { return m_nChannels;  }
		int             getDeviceId		() const { return m_deviceId;  }
		Image&			getImage        ()		 { return m_currentCameraImage; }
		GraphicsType	getFormat		() const { return m_format; }

	protected:

		// Protected methods
		void  			setNewFrameData ( unsigned char* data, unsigned int width, unsigned int height, GraphicsType format, int widthStep, bool swapRB = false );
		void  			setNewFrame     ( bool newFrame ) { m_newFrame = newFrame;    }  
		void  			setWidth        ( int width   )   { m_width   = width;  }
		void  			setHeight       ( int height  )   { m_height  = height; }
		void  			setFPS          ( int fps     )   { m_fps   = fps;  } 

		// Protected attributes
		int				m_deviceId;					///< Id of the device to capture from (starting at 0).

	private:
		// Attributes
		Ogre::Timer		m_timer;					///< Timer to check the capture fps
		AverageDouble	m_realFpsAverage;			///< Real fps average
		double			m_realFps;					///< Real captured fps
		Image			m_currentCameraImage; 		///< Image captured from the camera 
		Image			m_tempImage;				///< Image used to convert received frames to working format
		int             m_width, m_height;    		///< Capture resolution
		int             m_fps;                		///< Capture frames per second
		int             m_frameSize;          		///< Size in bytes of a single frame
		int             m_nChannels;          		///< Number of channels of the captured image
		GraphicsType	m_format;					///< Format of the frames (RGB or GRAYSCALE)
		bool            m_newFrame;           		///< True when the camera has a new frame captured
		bool            m_bIsValid;	          		///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called.
		bool			m_showFps;					///< Show real capture fps

	};

} // namespace Cing

#endif // _BaseCameraInput_H_
