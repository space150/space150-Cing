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

#ifndef _BlobFinder_H_
#define _BlobFinder_H_

// Precompiled headers
#include "Cing-Precompiled.h"


#include "ComputerVisionPrereqs.h"
#include "Blob.h"
#include <vector>

// Graphics
#include "graphics/GraphicsTypes.h"
#include "graphics/Image.h"

namespace Cing
{


/**
 * @internal
 * Class that analyzes an image to finds blobs (elements).
 * What it looks for in an image is bright areas, so typically
 * the image result of a background subtraction is a good input.
 * It also gives information about the found blobs.
 */
class BlobFinder
{
public:

	// Public types
	typedef std::vector< Blob > Blobs; ///< Contains a sequence of blobs

	// Constructor / Destructor
	BlobFinder();
	~BlobFinder();

	// Init / Release
	virtual void	end           ();

	// Blob related methods
	virtual void	update				( const Image& inImage );

	// Set methods
	void			setMinBlobArea	( float minArea ) { m_minBlobArea = minArea; }
	void			setMaxBlobArea	( float maxArea ) { m_maxBlobArea = maxArea; }
	virtual void	setMaxBlobs   	( unsigned int maxBlobs )	{ m_maxBlobs = maxBlobs; }

	// Query methods
	virtual bool	isValid       () const { return m_bIsValid; }
	int				getNumBlobs   	() const { return m_nBlobs; }

	// TODO check n valid
	Blob&			getBlobN      ( int n )  { return m_blobs[n]; }

	// Constant / static attributes
	static const unsigned int DEFAULT_MAX_BLOBS;

protected:

	// Private methods
	void          extractBlobsInformation();

	// Attributes
	Blobs				m_blobs;                ///< To store the found blobs
	int					m_nBlobs;
	float               m_minBlobArea;          ///< Min area (in pixels) that a blob should have to be considered
	float				m_maxBlobArea;          ///< Max area (in pixels) that a blob should have to be considered
	unsigned int		m_maxBlobs;             ///< Max number of blos to look for

	// OpenCV stuff
	CvMemStorage*		m_findContoursStorage;  ///< To store contour data (by opencv)
	CvSeq*				m_contour;              ///< To store opencv contours

	bool				m_bIsValid;				///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called.

};

} // namespace Cing

#endif // _BlobFinder_H_
