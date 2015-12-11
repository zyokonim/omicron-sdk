# Tutorial 8 - Using Unity #



This tutorial will demonstrate how to grab and use OmegaLib input data in Unity.

Currently Unity supports OmegaLib input through the following scripts:
  * InputServiceScript.cs
  * TouchScript.cs
  * MocapScript.cs
  * OmegaControllerScript.cs

All of these scripts along with an example Unity project with different scenes for touch, mocap, controller, and stereoscopic examples can be found on the svn under 'trunk/tutorials/tutorial8'.


### Descriptions of the Unity Examples ###


#### OmegaLib Example - Kinect.unity ####
  * Documentation pending

#### OmegaLib Example - NeuroSky.unity ####
  * The Cube object contains the NeuroSkyScript.
  * You can see the incoming data in the inspector as all the available data are public variables.
  * See the oinputserver protocol reference for a detailed decription of the variables.

#### OmegaLib Example - Stereoscopic.unity ####
  * Uses the HeadTrackedStereo prefab to showcase InterlacedStereoScript.js.
  * Also uses the MocapScript with Tracked Object ID 0 for head tracking.
  * Demos the use of the HeadTrackedStereo to provide viewer-centered perspective in interlaced stereo vision.

#### OmegaLib Example - Touch.unity ####
  * Several cubes sitting on a plane will change color if they are touched.
  * Clicking using the mouse will also send a touch event.

#### OmegaLib Example - Vicon.unity ####
  * This uses the MocapScript to move three objects around in 3D space.
  * The camera uses Tracked Object ID 0 and move around with the stereo glasses with markers.
  * The sphere moves based on Tracked Object ID 1 (Rigid bodies on a black frame labeled 1).
  * The cube moves based on Tracked Object ID 2 (Rigid bodies on a black frame labeled 2).

#### OmegaLib Example - Wiimote.unity ####
  * The cube labeled Wiimote uses WiimoteControllerScript.cs.
  * The cube will rotate based on WiiMotionPlus data (if available).
  * The cube will also move based on the nunchuk analog stick (if available).
  * The blue box will display white spheres representing the IR data.

#### OmegaLib Example - Xbox Controller.unity ####
  * This uses the XBox360ControllerScript to control a cube in the world called 'XCube'.
  * The left analog stick will move the cube along the X-Z plane along the ground.
  * The right analog stick will rotate the cube.
  * The triggers will move the cube up/down along the Y axis.

### Setting up the Input Service ###

InputServiceScript handles communication with OInputServer as well as streaming the apperperiate data to the other scripts listed above. The other scripts parse and store the data revalent to that input device.

  1. Create an empty gameobject and name it InputService.
  1. Drag InputServiceScript.cs on your InputService gameobject.
  1. When you click on the InputService gameobject, you will see the following options:

<p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/InputServiceScript.png' /></p><p align='middle'></p>

  * Input Server - IP address of the tracker machine where OInputServer is running.
  * Data Port - The port on the Unity machine where you want the data sent to.
  * Msg Port - The message port on the tracker machine that OInputServer is listening for clients.
  * Enable Input Service - Toggles the threaded network connection between Unity and OInputServer. This is useful to disable for testing without OInputServer.
  * NEC\_Wall\_LEFT/NEC\_Wall\_Right - Adjusts the scale of the touch data to work on half of the touch display only **(Touch Only)**.
  * Touch Time Out - Time in milliseconds before a touch is considered lost/dropped due to packet loss.

### Adding a Touch Object ###

> WIP

### Adding a Controller Object ###

1. Create a gameobject that you want to use with a controller.

2. Set the tag of that gameobject as "OmegaListener"
  * If you want to have your gameobject as a particular tag other than "OmegaListener" you can make this a child of your gameobject.
  * Create this tag if you haven't already using the Tag drop down menu and Add Tag.

> <p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/UnityAddTag.png' /></p><p align='middle'></p>

3. Create a new C# script that extends OmegaController Script. Let's call it MyControllerScript.

4. You'll notice a few public variables in the inspector view:
  * Controller ID is the controller ID you want this object to respond to. A unique ID assigned to each controller from the OmegaLib tracker. Typically if you're using only one controller, this will have an ID of 0. You will have to check the OInputServer output for which controller IDs were assigned.
  * Controller Type is what kind of controller the Controller ID corresponds to:
  * 0 = Xbox 360
  * 1 = PS3 Sixaxis
  * 2 = Wiimote

5. You can use the following accessor methods to get various data:
  * Vector3 getLeftAnalogStick()
  * Vector3 getRightAnalogStick()
  * Vector3 getNunchukAnalogStick()
  * float getTrigger()
  * int getButton( Button.[name](name.md) )
  * Button is an enumeration to access various buttons based on the controller type so replace name with one of the following:
    * Xbox360: A,B,X,Y,LB,RB,Back,Start,LA,RA,Up,Down,Left,Right
      * LA/RA are the left/right analog stick buttons
    * PS3: Cross,Circle,Square,Triangle,L1,[R1](https://code.google.com/p/omicron-sdk/source/detail?r=1),Select,Start,L3,[R3](https://code.google.com/p/omicron-sdk/source/detail?r=3),PS,Up,Down,Left,Right
      * L3/[R3](https://code.google.com/p/omicron-sdk/source/detail?r=3) are the left/right analog stick buttons
    * Wii: A,B,Plus,Home,Minus,One,Two,Up,Down,Left,Right,Z,C
  * Example: getButton( Button.A ) will return either 0 (button not pressed) or 1 (button pressed) for button A. If there is no A button for this particular controller, method will just return 0.
  * Analog sticks and triggers also have deadzone and sensitivity setters:
  * setTriggerSensitvity(float)
  * setLeftAnalogStickSensitivity(float)
  * setRightAnalogStickSensitivity(float)
  * setNunchukAnalogStickSensitivity(float)
  * setLeftAnalogStickDeadzone(Vector3)
  * setRightAnalogStickDeadzone(Vector3)
  * setNunchukAnalogStickDeadzone(Vector3)
  * Motion sensing controllers also have:
  * float getSixaxisPitch()
  * float getSixaxisRoll()
  * Vector3 getWiimoteAcceleration()
  * Vector3 getWiiMotionPlusAcceleration()

### Creating a Mocap Object ###

1. Attach reflective markers to the object you want to track. The markers should be positioned in a way that it has a unique appearence regardless of orientation.

2. Open a 'New Floating Workspace' from the Window menu.

<p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/Vicon01.png' /></p><p align='middle'></p>

3. Place your object in the view of the camera so that all markers appear in the 3D Persepective window.

<p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/Vicon02.png' /></p><p align='middle'></p>

4. Hold the Alt key and drag a window around the markers in the 3D Perspecive window.

<p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/Vicon03.png' /></p><p align='middle'></p>

5. All the markers should now be appear blue and have a raycast from the cameras (see image below).

<p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/Vicon04.png' /></p><p align='middle'></p>

6. Go to the Objects tab and under 'Create Object' at the bottom of the tab, enter the name in the text field and click 'Create'.

<p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/Vicon05.png' /></p><p align='middle'></p>

7. On the desktop double-click 'oinputserver config', scroll down to 'VRPNService' and add your object's name and a unique ID number to the list in the format shown below. This will be the ID you will use in Unity.

8. Double-click on the 'Start oinputserver' shortcut on the desktop to start the server.

### Adding a Mocap Object to Unity ###

See the above for creating rigid bodies and adding them to the rigid body list.

1. Create a gameobject to have tracked using the mocap script.

2. Set the tag of that gameobject to "OmegaListener"
  * Create this tag if you haven't already using the Tag drop down menu and Add Tag.

> <p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/UnityAddTag.png' /></p><p align='middle'></p>

3. Create a new C# script that extends MocapScript called MyMocapScript.

> <p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/UnityMyMocapScript.png' /></p><p align='middle'></p>

4. You can call functions like getPosition() and getOrientation() to get the mocap data as a Vector3. You can use the transform function shown in MocapExampleScript.cs.
  * You can also use getRawPosition() and getRawOrientation() to get Vector3 data directly from the tracker. These may not use the same coordinate system/origin as Unity.

5. Click on the gameobject you attached MyMocapScript to and check the inspector view. Set the variable My ID to the ID of the rigid body you want to track.

> <p align='middle'><img src='http://omegalib.googlecode.com/svn/wiki/UnityMocapScriptID.png' /></p><p align='middle'></p>

### Using the Stereoscopic Script with Head Tracking ###

To integrate stereo into your Unity project you will need the following:
  * OmegaLibUnityStereoscopic.unitypackage
  * InputServiceScript.cs
  * MocapScript.cs (Included in OmegaLibUnityStereoscopic.unitypackage)
  * Unity Pro (to preview stereo output)

OmegaLibUnityStereoscopic.unitypackage can be found on the svn under 'trunk/tutorials/tutorial8/Assets/Packages'

As of 10/10/11 the head tracked stereoscopic script is tested to work in Unity 3.1.0 up to 3.4.1
  * Windows: http://download.unity3d.com/download_unity/UnitySetup-3.4.1.exe
  * Mac: http://download.unity3d.com/download_unity/unity-3.4.1.dmg
  * Other versions are available simply by changing the version number.

The origin of the stereo camera is at the center of the screen, 1.5 Unity units (meters) above the floor. So when using this script, if the player is head-tracked and is standing on the floor, the floor should be translated by -1.5 in the Y direction.

First we'll look at using a pre-made camera package. Afterwards we'll look at manually adding the stereo script to you existing main camera.

#### Using the Pre-Made Package ####

1. Setup the InputService as described above.

2. Import the OmegaLibUnityStereoscopic package
  * Right-click in the Project tab and select 'Import Package'.
  * Click Import

3. Drag the HeadTrackedStereo prefab object into your scene

4. HeadTrackedStereo includes a main camera so disable/remove your main camera if you already have one.

5. The pre-made package comes with three cubes, the parent of which is 'Cube On' which represents a cube on the projection plane directly in front of the camera.

6. Your stereo view is now setup. Remember that you need Unity pro in order to preview this. Also the stereo perspective is calculated for the 2x2 stereo display in the classroom so the perspective may be off on your local machine.

#### Manually Adding Stereo to the Main Camera ####

1. Import the OmegaLibUnityStereoscopic package
  * Right-click in the Project tab and select 'Import Package'.
  * Click Import

2. Create a Mocap Object as described above. This will represent the player's head position.

3. Drag 'Scripts/InterlacedStereoScript.cs' on to your main camera.

4. Notice the public variables on your camera:
  * Enable Stereo - Toggles the stereo perspective on/off. (This will have of effect on the free version of Unity, you will have to disable the script to see the camera view. However this view will have a different perspective than the stereo view so use Unity Pro or test often on the VR system!)
  * Eye Position - Set this to the mocap object we created in step 2.
  * Focus Position - This is the focal point of the camera. This should always be at the mocap origin with local coordinates at 0,0,0. Check the HeadTrackedStereo prefab to see the proper relationship between the Eye Position, Main Camera, and Focus Position.
  * Stereo Material - Use the meterial under 'Materials/stereo3DMat'. Make sure this material's shader is set to 'Stereo 3D View Material'.

5. Your camera is now set for stereovision. Remember that you need Unity pro in order to preview this. Also the stereo perspective is calculated for the 2x2 stereo display in the classroom so the perspective may be off on your local machine.