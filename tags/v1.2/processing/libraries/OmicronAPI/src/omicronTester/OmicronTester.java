package omicronTester;
import omicronAPI.*;
import processing.core.*;

public class OmicronTester extends PApplet{
	OmicronAPI omicronManager;
	TouchListener touchListener;
	PApplet applet;
	
	// Override of PApplet init() which is called before setup()
	public void init() {
		super.init();
	  
		// Creates the OmicronAPI object. This is placed in init() since we want to use fullscreen
		omicronManager = new OmicronAPI(this);
	  
		// Removes the title bar for full screen mode (present mode will not work on Cyber-commons wall)
		omicronManager.setFullscreen(true);
	}
	
	public void setup(){
		// Depending on which version of Processing you're using:
		//size( screen.width, screen.height, OPENGL ); // 1.5.1
		//size( screenWidth, screenHeight, P3D ); // 2.0a1 - 2.0a5 (P3D renderer recommended if using the wall)
		size( displayWidth, displayHeight, P3D ); // 2.0a6+ - this is broken in 2.0a8 - recommend manually entering in resolution
		//size( 8160, 2304, P3D ); // Cyber-Commons wall
		
		// Make the connection to the tracker machine (uncomment this if testing without touch)
		//omicronManager.ConnectToTracker(7001, 7340, "131.193.77.104");
		  
		// Create a listener to get events
		touchListener = new TouchListener();
		  
		// Register listener with OmicronAPI
		omicronManager.setTouchListener(touchListener);

		// Sets applet to this sketch
		applet = this;
	}
	
	public void draw(){
		// Sets the background color
		background(24);

		// For event and fullscreen processing, this must be called in draw()
		omicronManager.process();
	}
	
	public void touchDown( float ID, float xPos, float yPos, float xWidth, float yWidth ){
	
	}
	
	public void touchMove( float ID, float xPos, float yPos, float xWidth, float yWidth ){
		
	}
	
	public void touchUp( float ID, float xPos, float yPos, float xWidth, float yWidth ){
		
	}
	
	// TouchListener ------------------------------------------------------------------------------
	class TouchListener implements OmicronTouchListener {
		 
		  // Called on a touch down event
		  // mousePressed events also call this with an ID of -1 and an xWidth and yWidth of 10.
		  public void touchDown(int ID, float xPos, float yPos, float xWidth, float yWidth){
		    fill(255,0,0);
		    noStroke();
		    ellipse( xPos, yPos, xWidth, yWidth );
		    
		    // This is an optional call if you want the function call in the main applet class.
		    // 'OmicronExample' should be replaced with the sketch name i.e. ((SketchName)applet).touchDown( ID, xPos, yPos, xWidth, yWidth );
		    // Make sure applet is defined as PApplet and that 'applet = this;' is in setup().
		    ((OmicronTester)applet).touchDown( ID, xPos, yPos, xWidth, yWidth );
		  }// touchDown
		  
		  // Called on a touch move event
		  // mouseDragged events also call this with an ID of -1 and an xWidth and yWidth of 10.
		  public void touchMove(int ID, float xPos, float yPos, float xWidth, float yWidth){
		    fill(0,255,0);
		    noStroke();
		    ellipse( xPos, yPos, xWidth, yWidth );
		    
		    ((OmicronTester)applet).touchMove( ID, xPos, yPos, xWidth, yWidth );
		  }// touchMove
		  
		  // Called on a touch up event
		  // mouseReleased events also call this with an ID of -1 and an xWidth and yWidth of 10.
		  public void touchUp(int ID, float xPos, float yPos, float xWidth, float yWidth){
		    fill(0,0,255);
		    noStroke();
		    ellipse( xPos, yPos, xWidth, yWidth );
		    
		    ((OmicronTester)applet).touchUp( ID, xPos, yPos, xWidth, yWidth );
		  }// touchUp
		  
	}// TouchListener
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		PApplet.main(new String[] { OmicronTester.class.getName() });
	}

}
