/**
 * ---------------------------------------------
 * OmicronTouchExample.pde
 * Description: Omicron processing example for the Cyber-commons wall
 *
 * Class: 
 * System: Processing 2.0.5 (beta), SUSE 12.1
 * Author: Arthur Nishimoto
 * Version: 1.0
 *
 * Version Notes:
 * 6/14/12      - Initial version
 * 6/19/12      - Added example for fullscreen, scaling, and touch
 * ---------------------------------------------
 */

import processing.net.*;
import omicronAPI.*;

OmicronAPI omicronManager;
MyTouchListener touchListener;

PFont font;

// Program timers
float startTime; // Time program was started
float programTimer; // Time since started

// Link to this Processing applet - used for touchDown() callback example
PApplet applet;

private boolean javaFrameSet = false; // Internal flag so that the frame position is only set once

// Override of PApplet init() which is called before setup()
public void init() {
  super.init();
  
  println("Connecting to tracker");
  
  // Use this constructor if not using tracking data (only fullscreen/scaling functions)
  //omicronManager = new OmicronAPI(this);
  
  // Use this constructor if using tracking data
  omicronManager = new OmicronAPI(this, 7000, 7340, "localhost");
  
  // Removes the title bar for full screen mode (present mode will not work on Cyber-commons wall)
  omicronManager.setFullscreen(true);
}

// Program initializations
void setup() {
  size( screenWidth, screenHeight, P3D ); // P3D renderer recommended if using the wall
  
  // Create a listener to get events
  touchListener = new MyTouchListener();
  
  // Register listener with OmicronAPI
  omicronManager.setTouchListener(touchListener);
  
  // Calculates the screen transformation
  omicronManager.calculateScreenTransformation(8160,2304);
  
  font = loadFont("ArialMT-48.vlw");
  textFont(font, 16);
  
  // Timer stuff
  startTime = millis() / 1000.0;
  
  // Sets applet to this sketch
  applet = this;
}// setup

void draw() {
  programTimer = millis() / 1000.0; // Update the timer

  // Sets the background color
  background(24);
  
  pushMatrix();
  boolean scaleScreen = true;
  if( scaleScreen ){
    // Applies the screen transformation
    translate( omicronManager.getScreenOffsetX(), omicronManager.getScreenOffsetY() );
    scale( omicronManager.getScreenScale() );
  }
  
  // Draws a box the size of the target screen size
  fill(0);
  rect( 0, 0, 8160, 2304 );
  
  // Basic debugging text
  fill(255);
  textAlign(LEFT);
  text("Timer :" + programTimer, 16, 16 * 1);
  text("FPS :" + frameRate, 16, 16 * 2);

  // For event and fullscreen processing, this must be called in draw()
  omicronManager.process();
  
  popMatrix();
}// draw

// See MyTouchListener on how to use this function call
void touchDown(int ID, float xPos, float yPos, float xWidth, float yWidth){
  println("Touch down from applet");
}// touchDown
