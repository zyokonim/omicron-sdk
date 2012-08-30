/**
 * ---------------------------------------------
 * OmicronTouchExample.pde
 * Description: Omicron Processing example for running a fullscreen touch application on the Cyber-Commons wall.
 *
 * Class: 
 * System: Processing 2.0a5, SUSE 12.1, Windows 7
 * Author: Arthur Nishimoto
 * Version: 1.0
 *
 * Version Notes:
 * 6/14/12      - Initial version
 * 6/19/12      - Added example for fullscreen, scaling, and touch
 * 6/20/12      - Cleaned up example
 * ---------------------------------------------
 */

import processing.net.*;
import omicronAPI.*;

OmicronAPI omicronManager;
TouchListener touchListener;

// Link to this Processing applet - used for touchDown() callback example
PApplet applet;

// Override of PApplet init() which is called before setup()
public void init() {
  super.init();
  
  // Creates the OmicronAPI object. This is placed in init() since we want to use fullscreen
  omicronManager = new OmicronAPI(this);
  
  // Removes the title bar for full screen mode (present mode will not work on Cyber-commons wall)
  omicronManager.setFullscreen(true);
}

// Program initializations
void setup() {
  // Depending on which version of Processing you're using:
  //size( screen.width, screen.height, OPENGL ); // 1.5.1
  //size( screenWidth, screenHeight, P3D ); // 2.0a1 - 2.0a5 (P3D renderer recommended if using the wall)
  //size( displayWidth, displayHeight, P3D ); // 2.0a6+ - this is broken in 2.0a8 - recommend manually entering in resolution
  size( 8160, 2304, P3D ); // Cyber-Commons wall
  
  // Make the connection to the tracker machine
  omicronManager.ConnectToTracker(7001, 7340, "131.193.77.104");
  
  // Create a listener to get events
  touchListener = new TouchListener();
  
  // Register listener with OmicronAPI
  omicronManager.setTouchListener(touchListener);

  // Sets applet to this sketch
  applet = this;
}// setup

void draw() {
  // Sets the background color
  background(24);

  // For event and fullscreen processing, this must be called in draw()
  omicronManager.process();
}// draw

// See TouchListener on how to use this function call
// In this example TouchListener draws a solid ellipse
// Ths functions here draws a ring around the solid ellipse

// NOTE: Mouse pressed, dragged, and released events will also trigger these
//       using an ID of -1 and an xWidth and yWidth value of 10.
void touchDown(int ID, float xPos, float yPos, float xWidth, float yWidth){
  noFill();
  stroke(255,0,0);
  ellipse( xPos, yPos, xWidth * 2, yWidth * 2 );
}// touchDown

void touchMove(int ID, float xPos, float yPos, float xWidth, float yWidth){
  noFill();
  stroke(0,255,0);
  ellipse( xPos, yPos, xWidth * 2, yWidth * 2 );
}// touchMove

void touchUp(int ID, float xPos, float yPos, float xWidth, float yWidth){
  noFill();
  stroke(0,0,255);
  ellipse( xPos, yPos, xWidth * 2, yWidth * 2 );
}// touchUp
