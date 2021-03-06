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
  Inc., 59 Temple Place, Su ite 330, Boston, MA  02111-1307  USA
*/

#ifndef _Cing_CommonUtilsIncludes_h_
#define _Cing_CommonUtilsIncludes_h_

// Precompiled headers
#include "Cing-Precompiled.h"


/// @file This file contains the includes of all the common system utils of the library

// common general utils
#include "common/CommonTypes.h"
#include "common/Exception.h"
#include "common/LogManager.h"
#include "common/Release.h"
#include "common/MathUtils.h"
#include "common/SystemUtils.h"
#include "common/eString.h"
#include "common/PerlinNoise.h"
#include "common/CommonUserAPI.h"
#include "common/XMLElement.h"
#include "common/ResourceManager.h"

// standard general utils
#include <cstdio>

//TODO: Review why when introducing OpenCV 2.1 all warning coming from casts appear in VSTudio
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
	#define _CRT_SECURE_NO_WARNINGS
	#pragma warning (disable : 4244 ) 
#endif

#endif // _CommonUtilsIncludes_h_
