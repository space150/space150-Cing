// Code from: http://processing.org/learning/basics/distance2d.html

// Move the mouse across the image to obscure and reveal the matrix.
// Measures the distance from the mouse to each square and sets the
// size proportionally.

#include "Cing.h"
CREATE_APPLICATION( "Cing" );

float max_distance;

void setup()
{
	size(200, 200); 
	smooth();
	noStroke();
	max_distance = dist(0, 0, width, height);
}

void draw()
{
	background(51);
  
	for(int i = 0; i <= width; i += 20) {
		for(int j = 0; j <= height; j += 20) {
			float size = dist(mouseX, mouseY, i, j);
			size = size/max_distance * 66;
			ellipse(i, j, size, size);
		}
	}
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
}