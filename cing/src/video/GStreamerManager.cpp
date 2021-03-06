/*
This source file is part of the Cing project
For the latest info, see http://www.cing.cc

Copyright (c) 2008-2010 Julio Obelleiro and Jorge Cano

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

// Precompiled headers
#include "Cing-Precompiled.h"

#include "GStreamerManager.h"

// GStreamer
#include <gst/gst.h>

// Common
#include "common/CommonUtilsIncludes.h"

namespace Cing
{
	// Static member init
	String	GStreamerManager::m_pluginsPath		= "";
	bool	GStreamerManager::m_GSInitialized	= false;

	// Static methods
	void GStreamerLOG(const gchar* string)
	{
		LOG(" GStreamer LOG: %s", string );
	}

	void GStreamerErrorLOG(const gchar* string)
	{
		LOG_ERROR(" GStreamer ERROR: %s", string );
	}


	/**
	 * Initializes GStreamer common stuff
	 * @return true if the initialization was ok
	 */
	bool GStreamerManager::initGStreamer()
	{
		if ( m_GSInitialized )
			return true;

		// Load Cing Config file to get the GStreamer plugins directory
		XMLElement xml;
		xml.load( "CingConfig.xml" );

		// Get cing data folder (the root is CingConfig)
		if ( xml.isValid() )
		{
			XMLElement pluginsFolder = xml.getChild( "GStreamer_Plugins_Folder" );
			m_pluginsPath = pluginsFolder.getStringAttribute( "relativePath" );
			if ( m_pluginsPath.empty() )
			{
				LOG_ERROR( "GStreamer_Plugins_Folder tag not found in data/CingConfig.xml file. You may have an old version of this file." );
				return false;
			}

			// Check if folder exists
			if ( folderExists( m_pluginsPath ) == false )
			{
				LOG_ERROR( "GStreamer_Plugins_Folder found in CingConfig.xml does not exist. The folder specified is: %s", (ResourceManager::userDataPath + m_pluginsPath).c_str() );
				return false;
			}

			// Set env variable
			g_setenv( "GST_PLUGIN_PATH", m_pluginsPath.c_str(), 1 );

			LOG( "GStreamer plugins path: %s", m_pluginsPath.toChar() );
		}

		// Initialize Gstreamer with extended arguments
		gboolean verbose = TRUE;
		GOptionContext *ctx;
		GError *err = NULL;
		GOptionEntry entries[] ={
									{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
									{ NULL }
								};

		/* we must initialise the threading system before using any
		* other GLib funtion, such as g_option_context_new() */
		if (!g_thread_supported ())
			g_thread_init (NULL);

		ctx = g_option_context_new ("- Your application");
		g_option_context_add_main_entries (ctx, entries, NULL);
		g_option_context_add_group (ctx, gst_init_get_option_group ());
		if (!g_option_context_parse (ctx, NULL, NULL, &err)) 
		{
			LOG_ERROR ("GStreamer: Failed to initialize: %s\n", err->message);
			g_error_free (err);
			return false;
		}

		// Retrieve version information
		guint major, minor, micro, nano;
		gst_version (&major, &minor, &micro, &nano);
		LOG( "Gstreamer Version: %d.%d.%d.%d", major, minor, micro, nano );

		// Set GStreamer to log through our Logger
		g_set_print_handler(&GStreamerLOG);
		g_set_printerr_handler(&GStreamerErrorLOG);

		// GStreamer initialized
		m_GSInitialized = true;
		LOG( "GStreamer Initialized succesfully" );

		return true;
	}

} // namespace
