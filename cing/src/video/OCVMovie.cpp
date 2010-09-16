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

#include "OCVMovie.h"

// Common
#include "common/ResourceManager.h"
#include "common/LogManager.h"
#include "common/MathUtils.h"
#include "common/CommonUtilsIncludes.h"

// Graphics
#include "graphics/GraphicsTypes.h"
#include "graphics/Image.h"

// Open CV for video capture
#include "OpenCV/cv.h"
#include "OpenCV/highgui.h"

namespace Cing
{

/**
 * Default constructor.
 */
OCVMovie::OCVMovie() : 
		m_capture( NULL ),
		m_width( 0 ),
		m_height( 0 ),
		m_loop( false ),
		m_playing( false ),
		m_finished( false )
{
}


/**
 * Destructor. It closes the video file.
 */
OCVMovie::~OCVMovie()
{
  end();
}

/**
 * Loads a movie from a file.
 * @param fileName The movie file's name.
 *
 * @return true if the video was correctly loaded, false otherwise
 */
bool OCVMovie::load(const char* fileName )
{
	// Compose full path
	std::string path = ResourceManager::userDataPath + fileName;

	// Init opencv capture device
	m_capture = cvCaptureFromFile( path.c_str() );
	if ( !m_capture )
	{
	  LOG_ERROR("Error loading video, maybe it does not exist?, Is it AVI format?" );
			return false;
	}

	// Store the filename
	m_fileName = fileName;

	// Message
	LOG( "Movie %s succesfully loaded", fileName );

	// Store video data
	m_width     = static_cast<int>( cvGetCaptureProperty( m_capture, CV_CAP_PROP_FRAME_WIDTH ) );
	m_height    = static_cast<int>( cvGetCaptureProperty( m_capture, CV_CAP_PROP_FRAME_HEIGHT ) );
	m_fps		= static_cast<int>( cvGetCaptureProperty( m_capture, CV_CAP_PROP_FPS ) );
	m_nFrames	= cvGetCaptureProperty( m_capture, CV_CAP_PROP_FRAME_COUNT );

	// Set the current fps for the playback speed
	setFps( m_fps );

	return true;
}


/**
 * Closes the movie.
 */
void OCVMovie::end()
{
  if ( isValid() )
  {
      // Release capture device
      cvReleaseCapture( &m_capture );
  }
}

/**
 * @brief Plays the video. From now on you can grab frames calling: video.getFrame()
 */
void OCVMovie::play()
{
	m_playing			= true;
	m_loop				= false;
	m_firstFrame	= true;
	m_finished = false;

	// Start timer to control the playback speed
	cvSetCaptureProperty( m_capture, CV_CAP_PROP_POS_FRAMES, 0 );
	m_timer.reset();
}

/**
 * @brief Plays the video in loop. From now on you can grab frames calling video.getFrame()
 *
 * @param
 */
void OCVMovie::loop()
{
	m_playing			= true;
	m_loop				= true;
	m_firstFrame	= true;
	m_finished = false;

	// Start timer to control the playback speed
	m_timer.reset();
}

/**
* @brief Stops the video from looping
*
* @param
*/
void OCVMovie::noLoop()
{
	m_playing			= true;
	m_loop				= false;
	m_firstFrame	= true;
	m_finished = false;

	// Start timer to control the playback speed
	m_timer.reset();
}

/**
 * @brief Stops the video from playing. Subsequent calls to video.getFrame() will return no frame
 *
 */
void OCVMovie::stop()
{
	m_playing = false;
	m_loop		= false;
}

/**
 * @brief Sets the fps of the video playback.
 *
 * @param
 */
void OCVMovie::setFps( int fps )
{
	// Calculate time step between frames (in microseconds)
	m_timeBetweenFramesMs = (unsigned long)(1000000.0 / (double)fps);

	// Store the new value
	m_fps = fps;
}

/**
 * Obtains a frame from the video (if it is available -> it depends on the current frames per second)
 * @param frame The image where to store the frame.
 */
void OCVMovie::read( Image &image )
{
	// Check if video is playing
	if ( !m_playing || m_finished )
	{
		LOG_ERROR( "Trying to get frame of a video that is not playing" );
		return;
	}

	// Check state
	if ( !isValid())
	{
		LOG_ERROR( "Movie(%s): could not get the frame, because the movie has not been loaded", m_fileName.c_str( ) );
		return;
	}

	// Do nothing if no new frame (depends on the current frames per second)
	//if ( !newFrame() && !m_firstFrame )
	//	return;

	// Set current frame to ensure correct synchronization
	double elapsedMicroSecs = m_timer.getMicroseconds();
	double currentFrame		=  elapsedMicroSecs / m_timeBetweenFramesMs;
	currentFrame			= (double)constrain( (float)currentFrame, 0.0f, (float)m_nFrames-1 );
	cvSetCaptureProperty( m_capture, CV_CAP_PROP_POS_FRAMES, currentFrame );

	// Set first frame flag to false
	m_firstFrame = false;

	// Get frame from video
	IplImage*   frame = cvQueryFrame( m_capture );
	if ( !frame )
	{
	  LOG_ERROR( "Error getting frame from video" );
			return;
	}

	// Image format of the captured image
	GraphicsType format = frame->nChannels == 1? GRAYSCALE: RGB;

	// Check if the target image is valid (if not -> init it)
	if ( !image.isValid() )
		image.init( frame->width, frame->height, format );

	// Set the image data (flipping it vertically)
	cvConvertImage( frame, &image.getCVImage(), CV_CVTIMG_FLIP );
	image.setUpdateTexture( true );

	// If it was the last frame and we have to loop -> restar the video
	if ( equal( (float)cvGetCaptureProperty( m_capture, CV_CAP_PROP_POS_AVI_RATIO ), 1.0f ) )
	{
	  // Restart video
	  if ( m_loop )
	  {
		  cvSetCaptureProperty( m_capture, CV_CAP_PROP_POS_AVI_RATIO, 0 );
	  }
	  // Movie finished
	  else
	  {
		  m_finished = true;
	  }
	}

	// Reset the frames timer
	//m_timer.reset();
}

/**
 * Returns true if the video is correctly initialized.
 * @return True if the video is correctly initialized.
 */
bool OCVMovie::isValid() const
{
    return m_capture != NULL;
}

/**
 * @brief Returns true if a new frame is available (depending on the current frames per second)
 *
 * @return true if a new frame is available (depending on the current frames per second)
 */
bool OCVMovie::newFrame()
{
	return m_timer.getMicroseconds() >= m_timeBetweenFramesMs;
}

}
