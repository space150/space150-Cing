/*
This source file is part of the Cing project
For the latest info, see http://www.cing.cc

Copyright (c) 2008-2009 Julio Obelleiro and Jorge Cano

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

#ifndef _MediaPlayerVLC_H_
#define _MediaPlayerVLC_H_

// Precompiled headers
#include "Cing-Precompiled.h"

// VLC
#include <vlc/vlc.h>
#include "vlc/libvlc_structures.h"
#include "vlc/libvlc_events.h"

// Graphics
#include "graphics/Image.h"

// Extern libs
#undef nil
#include "PTypes/include/pasync.h" // for portable mutex

// Ogre
#include "OgreTimer.h"

namespace Cing
{

	/// Stores frames coming from VLC. The mutex is used to prevent multi threading issues
	struct VLCFrameContainer
	{
		Image                 image;
		pt::mutex             mutex;
	};

	/**
	* Media player that uses VLC to decode and play video/audio
	*/
	class MediaPlayerVLC
	{
	public:

		// Constructor / Destructor
		MediaPlayerVLC();
		~MediaPlayerVLC();

		// Init / Release / Update
		void    init    ();
		void    load    ( const char* fileName );
		void    end     ();
		void    update  ();
		Image&  getImage() { return m_frameContainer.image; }

		// Query methods
		bool    isValid   () const { return m_bIsValid; }
		bool    isPlaying ();
		float   duration  () const { return m_videoDuration / 1000.0f; }
		float   time      ();
		int     getWidth  () const { return m_videoWidth; }
		int     getHeight () const { return m_videoHeight; }

		// Media control
		void    play    ();
		void    loop    ();
		void    stop    ();
		void    pause   ();
		void    setLoop ( bool loop ) { m_loop = loop; }
		void    noLoop  () { setLoop( false  ); }
		void    jump    ( float whereInSecs );
		void    speed   ( float rate );

		// Audio Specific Control
		void  toggleMute();
		void  setVolume ( float volume );

		// Media control to respond to VLC events
		void endOfFileReached();

	private:


		// Internal methods
		void  checkVLCException();
		bool  getVideoSize();


		// Static members common to all players
		static String			m_pluginsPath;


		// VLC attributes
		libvlc_exception_t    	m_vlcException;
		libvlc_instance_t     	*m_libvlc;
		libvlc_media_t        	*m_media;
		libvlc_media_player_t 	*m_mediaPlayer;
		libvlc_event_manager_t	*m_eventManager;

		VLCFrameContainer		m_frameContainer; ///< Stores the image frames decoded by VLC

		// Media info
		String					m_fileName;
		String					m_filePath;
		libvlc_time_t         	m_videoDuration;
		float                 	m_videoFps;
		int                   	m_videoWidth;
		int                   	m_videoHeight;

		// Playback Settings
		bool                  	m_loop;
		bool                  	m_loopOnNextUpdate;

		// Other stuff
		Ogre::Timer			  	m_timer;
		bool                  	m_bIsValid;	      ///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called.

	};

} // namespace

#endif // _MediaPlayerVLC_H_