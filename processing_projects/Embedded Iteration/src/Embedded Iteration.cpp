// Code from: http://processing.org/learning/basics/embeddediteration.html

// Embedding "for" structures allows repetition in two dimensions.

#include "Cing.h"
CREATE_APPLICATION( "Cing" );

void setup()
{
	float box_size = 11; 
	float box_space = 12; 
	int margin = 7; 

	size(200, 200); 
	background(0); 
	noStroke(); 

	// Draw gray boxes 
	for (int i = margin; i < height-margin; i += box_space){
		if(box_size > 0){
			for(int j = margin; j < width-margin; j+= box_space){
				fill(255-box_size*10);
				rect(j, i, box_size, box_size);
			}
			box_size = box_size - 0.6;
		}
	}
}

void draw()
{
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