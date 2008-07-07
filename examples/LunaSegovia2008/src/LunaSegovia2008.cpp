#include "Vision.h"
#include "SegoviaParticleSystem.h"
#include "PathStorage.h"

CREATE_APPLICATION( "Play the Magic - Segovia 2008 - Noche de La Luna Llena" );

std::list< SegoviaParticleSystem* > pSystems;
std::list< PathStorage* >						paths;
bool																debugPaths, creatingNewPath;

// CV
BackgroundSubtraction bgSubtraction;	// Object to perform Background subtraction
BlobFinder						bfinder;				// Object to perform blob finding
Movie									movie;
Image									movieFrame;
Image									img;						// Image to draw the result of the background subtraction

void savePathsFile( const std::string& fileName )
{
	std::ofstream f ( fileName.c_str() );

	// Write the number of paths
	f << paths.size() << std::endl;

	// Write the paths
	std::list< PathStorage* >::iterator it = paths.begin();
	for ( ; it != paths.end(); ++it )
		f << *(*it)	;
}

void loadPathsFile( const std::string& fileName )
{
	std::ifstream f ( fileName.c_str() );

	// Read number of paths
	int nPaths;
	f >> nPaths;

	// Read paths
	for ( int i = 0; i < nPaths; ++i )
	{
		paths.push_back( new PathStorage( true ) );
		f >> *paths.back();
	}

}

void setup()
{
	showFps( true );
	//showDebugOutput( true );

	// Disable camera control
	useMouseCameraControl( false );
	useKeyboardCameraControl( false );

	// Init vars
	debugPaths			= true;
	creatingNewPath = false;

	// Load paths file
	std::string fileName = dataFolder + "Paths.txt";
	if ( fileExists( fileName ) )
		loadPathsFile( fileName );

	//Setup CV
	movie.load( "test_Segovia.avi" );
	movie.loop();
	movieFrame.init(  320, 240, RGB );
 	img.init( 320, 240, RGB );
	img.flipVertical();
	//movieFrame.flipVertical();
}

void draw()
{

	//Draw CV
	movie.read( movieFrame );
	// background subtraction
	bgSubtraction.update( movieFrame, img);
	// Find blobs
	bfinder.update( img );
	// Draw images
	movieFrame.draw2d( 0,0 );

	int nBlobs = bfinder.getNumBlobs();
	for ( int i = 0; i < nBlobs; i++ )
	{
		// get blob
		Blob& blob = bfinder.getBlobN( i );
		// draw it in the image
		img.rect( blob.bbox.x, blob.bbox.y, blob.bbox.x + blob.bbox.width, blob.bbox.y + blob.bbox.height );
	}

	img.draw2d( width/2 - img.getWidth()/2, height - img.getHeight() );
	// Update systems and delete dead ones
	std::list< SegoviaParticleSystem* >::iterator it = pSystems.begin();
	while( it != pSystems.end() )
	{
		SegoviaParticleSystem* pSystem = *it;
		if ( pSystem && !pSystem->update() )
		{
			it = pSystems.erase( it );
			delete pSystem;

			// if it was the last -> do nothing else
			if ( pSystems.empty() )
				break;
		}
		else
			++it;
	}

}

void end()
{
	// Save paths
	std::string fileName = dataFolder + "Paths.txt";
	if ( fileExists( fileName ) )
	{
		if ( MessageBox( NULL, "Paths.txt file already exists. Do you want to overwrite it?", "Paths save", MB_OKCANCEL ) == IDOK )
			savePathsFile( fileName );
	}
	else
		savePathsFile( fileName );
}

void mousePressed()
{
	if ( mouseButton == LEFT )
	{
		// Creating new path
		if ( creatingNewPath )
		{
			paths.back()->addPoint( mouseX, height - mouseY, 0 );
		}
		// Testing particles
		else
		{
			// config
			int nLines = 3;
			
			// Create new particle system
			pSystems.push_back( new SegoviaParticleSystem( mouseX, height - mouseY, 0, nLines, paths ) );
		}
	}
}

void mouseMoved()
{
}

void mouseReleased()
{
}

void keyPressed()
{
	// Drawing modes
	if ( key == '1' )
		setRenderMode( DRAW_WIREFRAME );
	else if ( key == '2' )
		setRenderMode( DRAW_SOLID );
	else if ( key == '3' )
		setRenderMode( DRAW_POINTS );

	// Configuration

	// Create new path
	else if ( key == 'p' )
	{
		// Start new path
		if ( !creatingNewPath )
		{
			creatingNewPath = true;
			paths.push_back( new PathStorage( true ) );
		}
		// End of path
		else
			creatingNewPath = false;
	}
	// Debug paths
	else if ( key == 'd' )
	{
		debugPaths = !debugPaths;
		std::for_each( paths.begin(), paths.end(), std::bind2nd( std::mem_fun( &PathStorage::setVisible ), debugPaths ) );
	}

	// Store background when space bar is pressed
	if ( key == ' ' )
		bgSubtraction.storeBackground( movieFrame );
}
