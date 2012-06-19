/**
 * ---------------------------------------------
 * OmicronFullscreenExample.pde
 * Description: Omicron processing example for the Cyber-commons wall
 *
 * Class: 
 * System: Processing 2.0.5 (beta), SUSE 12.1
 * Author: Arthur Nishimoto
 * Version: 1.0
 *
 * Version Notes:
 * 6/14/12      - Initial version
 * 6/19/12      - Added example for fullscreen and screen scaling for Cyber-commons
 * ---------------------------------------------
 */

import processing.net.*;
import omicronAPI.*;

OmicronAPI omicronManager;

PFont font;
int fontSize = 16;

// Program timers
float startTime; // Time program was started in seconds
float programTimer; // Time since started

// Screen Scaling
// This is not necessary for rendering at fullscreen, but can be helpful for previewing
// the sketch at the target aspect ratio.
boolean scaleScreen = false;
float targetWidth = 8160;
float targetHeight = 2304;
  
// Override of PApplet init() which is called before setup()
public void init() {
  super.init();

  // Use this constructor if not using tracking data (only fullscreen/scaling functions)
  omicronManager = new OmicronAPI(this);
  
  // Use this constructor if using tracking data
  //omicronManager = new OmicronAPI(this, 7000, 7340, "localhost");
  
  // Removes the title bar for full screen mode (present mode will not work on Cyber-commons wall)
  // This is called in init() to avoid Java frame errors
  omicronManager.setFullscreen(true);
}

// Program initializations
void setup() {
  size( screenWidth, screenHeight, P3D ); // P3D renderer recommended if using the wall
  
  // Calculates the screen transformation
  // This must be called after size since it uses screenWidth and screenHeight
  omicronManager.calculateScreenTransformation( targetWidth, targetHeight );
  
  // Font initialization
  font = loadFont("ArialMT-48.vlw");
  textFont(font, fontSize);
  
  // Timer stuff
  startTime = millis() / 1000.0; // Convert from millis to seconds
}// setup

void draw() {
  programTimer = millis() / 1000.0; // Update the timer

  // Sets the background color
  background(64);
  
  pushMatrix();
  if( scaleScreen ){
    // Applies the screen transformation
    translate( omicronManager.getScreenOffsetX(), omicronManager.getScreenOffsetY() );
    scale( omicronManager.getScreenScale() );
  }
  // Draw stuff...
  
  // Draws a box the size of the target screen size
  fill(0);
  rect( 0, 0, targetWidth, targetHeight );
  
  // Basic debugging text
  fill(255);
  textAlign(LEFT);
  text("Timer :" + programTimer, fontSize, fontSize * 1);
  text("FPS :" + frameRate, fontSize, fontSize * 2);
  if( !scaleScreen )
    text("Resolution: "+width+" , "+height+" (Not scaled)", fontSize, fontSize*3);
  else
    text("Resolution: "+width+" , "+height+" (Scaled from "+targetWidth+" , "+targetHeight+" by "+omicronManager.getScreenScale()+" )", fontSize, fontSize*3);
    
    
  // For event and fullscreen processing, this must be called in draw()
  omicronManager.process();
  popMatrix();
}// draw
