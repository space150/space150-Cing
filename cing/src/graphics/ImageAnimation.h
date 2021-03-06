/*
This source file is part of the Cing project
For the latest info, see http://www.cing.cc

  Copyright (c) 2006-2011 Julio Obelleiro and Jorge Cano

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

#ifndef _Image_Animation_H_
#define _Image_Animation_H_

// Precompiled headers
#include "Cing-Precompiled.h"


#include "GraphicsPrereqs.h"
#include "Image.h"

namespace Cing
{

	/**
	* @brief Stores an image animation that can be played/stopped...
	* Supported image formats are: .bmp, .jpg, .gif, .raw, .png, .tga and .dds.
	* @note Files must be named in sequence
	*/
	class ImageAnimation: public Image
	{
	public:

		// Init / Release / Update / Save / Clone
		bool		load	( const std::string& baseName, int nFrames, float durationSecs );

		// Playback control
		void		play();
		void		stop();

		// Getters
		float		getAnimationDuration();
		int			getNumberFrames(){return m_nFrames;}

		//Setters
		bool setCurrentFrame(int newCurrentFrame);

	private:
		int m_nFrames;
	};

} // namespace Cing

#endif // _Image_Animation_H_
