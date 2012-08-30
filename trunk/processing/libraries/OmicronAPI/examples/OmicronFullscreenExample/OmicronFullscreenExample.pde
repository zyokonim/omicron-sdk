/**
 * ---------------------------------------------
 * OmicronFullscreenExample.pde
 * Description: Omicron Processing example for running a fullscreen application on the Cyber-Commons wall.
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
  //size( screenWidth, screenHeight, P3D ); // 2.0a1 - 2.0a5
  //size( displayWidth, displayHeight, P3D ); // 2.0a6+ - this is broken in 2.0a8 - recommend manually entering in resolution
  size( 8160, 2304, P3D ); // Cyber-Commons wall
}// setup

void draw() {
  // Sets the background color
  background(24);

  // For event and fullscreen processing, this must be called in draw()
  omicronManager.process();
}// draw
