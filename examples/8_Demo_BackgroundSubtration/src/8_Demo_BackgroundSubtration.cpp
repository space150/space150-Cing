#include "Vision.h"

CREATE_APPLICATION( "Vision Demo" );

BackgroundSubtraction bgSubtraction;
Capture								camera;	// Camera to capture video input
Image									img;		// Image to draw the result of the background subtraction

void setup()
{
	// Init camera and image
	camera.init( 0, 320, 240, 25, GRAYSCALE );
	img.init( 320, 240, GRAYSCALE );

	// Store background 
	bgSubtraction.storeBackground( camera.getImage() );
}

void draw()
{
	// Update the camera
	camera.update();
	camera.getImage().draw2d( 0, 0 );

	// background subtraction
	bgSubtraction.update( camera.getImage(), img);
	img.draw2d( 320, 0 );
}

void end()
{
}

void mousePressed()
{
}

void mouseMoved()
{
}

void mouseReleased()
{
}

void keyPressed()
{
	if ( key == ' ' )
		bgSubtraction.storeBackground( camera.getImage() );
}