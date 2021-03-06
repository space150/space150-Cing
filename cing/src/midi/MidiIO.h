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

#ifndef _MidiIO_H_
#define _MidiIO_H_

// Precompiled headers
#include "Cing-Precompiled.h"

#include "midi/MidiPrereqs.h"

#include "gui/GUIManagerCEGUI.h"

namespace Cing
{
/**
 * @internal
 * TODO: maybe we can make the same interface of MidiIO??
 * MidiIO is the base class for managing the available midi ports.
 * It provides you with methods to get information on your ports 
 * and to open them. To start use the printDevices
 * method to get all devices available on your system.
 */

class MidiIO
{
public:

	// Constructor / Destructor
	MidiIO();
	~MidiIO();

	// Init / Release / Update
	bool        init        ();
	void        end         ();
	void        update      ();

	// Query methods
	bool        isValid     () const	{ return m_bIsValid; }
	
	// Prints  input midi devices.
	void				printInputDevices();
	// Prints  output midi devices.
	void				printOutputDevices();
	// Prints  all midi devices.
	void				printDevices();

	// Use this Method to open an input device.
	void				openInput(int inputDeviceNumber);
	// Close an open MIDI connection (if one exists). 
	void				closeInput();

	static void	onMessage( double deltatime, std::vector< unsigned char > *message, void *userData );
	void				plug     ( void (*callbackFunction) ( double deltatime, std::vector< unsigned char > *message, void *userData ) );
	RtMidiIn*   m_MidiIn;

private:

	RtMidiOut*   m_MidiOut;
	bool         m_bIsValid;	        ///< Indicates whether the class is valid or not. If invalid none of its methods except init should be called.
};

} // namespace Cing

#endif // _MidiIO_H_
