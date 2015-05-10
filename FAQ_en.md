

# Installation #

## Which version ("for Kinect SDK" or "for OpenNI") should I use? ##

  * "For Kinect SDK" version is recommended for those who:
    * have Kinect and Windows 7 and want to run the application with a minimum burden
  * "For OpenNI" version is recommended for those who:
    * want to run the application on Windows Vista or earlier OSes that Kinect SDK does not support
    * cannot install Kinect SDK for some other reasons such as avoiding negative impacts on the existing OpenNI applications
    * want to try other sensors than Kinect
    * want to apply different skeleton tracking algorithms and compare

## What hardware do I need? ##

  * I cannot tell you exactly rather than Kinect, fairly powerful CPU, and fairly powerful 3D-enabled GPU. See WorkingConfigurations for the known valid configurations.
  * The GPU (supposedly) needs to support OpenGL 2.x, programmable shaders, and GL\_ARB\_vertex\_array\_object extension. If the GPU does not satisfy this requirement, the program ends saying "This GPU does not support shaders".

## How do I download the required software? ##

  * Kinect SDK --> See [KinectSDK\_Downloads](KinectSDK_Downloads.md)
  * OpenNI --> See [OpenNI\_Downloads](OpenNI_Downloads.md)

# It doesn't work! #

## Exits with an error displayed on the console ##

### "Failed: Can't create any node of the requested type!" appears (for OpenNI) ###

  * Possibly Kinect is not connected.
  * Possibly NITE is not installed.
    * If you think you already installed NITE, possibly the installation is corrupted. Uninstalling & reinstalling NITE could solve the issue.
  * Possibly Microsoft's official Kinect SDK is installed. See WorkingConfigurations for (a sort of) workaround.

### "Failed: Unknown USB device speed!" appears (for OpenNI) ###

  * Possibly Kinect is not connected to the power supply.

### "Failed: File not found!" appears (for OpenNI) ###

  * Possibly the current directory (working directory) does not contain "data" directory. Put kinect-ultra.exe and "data" directory at the same location, and launch it with the current directory set there.
  * If you attempt to run this program on VC++, set working directory to .. in the debugger settings. You need to do it manually because this setting is not saved to VC project files, sln or vcproj. (sigh...)

### "Failed: This GPU does not support shaders" appears ###

  * Possibly GPU and graphics driver does not support the required feature called "programmable shader." This feature is supported by OpenGL 2 or later. If the OpenGL version that appears right above the error message shows less than 2.0, this is the cause. This may be solved by upgrading the graphics driver.

### "Sorry this program cannot run because VAOs are not supported" appears ###

  * Possibly GPU and graphics driver does not support the required feature called "VAO." This feature is supported by OpenGL 3 or later. OpenGL 2 is also OK if your GPU and graphics driver supports VAO extension. This may be solved by upgrading the graphics driver.

## Exits with an error displayed in a popup dialog ##

### "Kinect10.dll is missing" appears (for Kinect SDK) ###

  * Possibly Kinect SDK is not installed. See [KinectSDK\_Downloads](KinectSDK_Downloads.md) for details.
  * Use Kinect SDK V1.0. Kinect SDK Beta is not supported.

### "OpenNI.dll is missing" appears (for OpenNI) ###

  * Possibly OpenNI 32-bit is not installed. See [OpenNI\_Downloads](OpenNI_Downloads.md) for details.
    * If you believe OpenNI is installed but see this error, OpenNI 64-bit may possibly be installed. Use OpenNI 32-bit. Whether your OS is 64-bit or 32-bit, OpenNI 32-bit works fine. You will not experience any performance drawback.
    * Maybe I am over-simplifying, but there are only few advantages in using OpenNI 64-bit such as you can run a program that requires a huge amount of memory. As I do not see any advantages for my program, it has not been my priority to build 64-bit version of my program. I may do it when I get some time in the future, but please use OpenNI 32-bit for the time being.

### "MSVC(foobar).dll is missing" or "side b yside configuration is incorrect" appears ###

  * Download Visual C++ 2010 Runtime from here: http://www.microsoft.com/en-us/download/details.aspx?id=5555

## Terminated abnormally ##

  * I got some reports of this kind, but the cause is not clear. This may be solved by following actions.
    * Use the latest kinect-ultra.
    * Use the latest graphics driver.
  * I will investigate the reported issues as far as I can, but it is actually very difficult to track down the problems dependent on GPU and graphic driver without having the exact environment on which the issue can be reproduced. I would appreciate your help such as tracing the execution by the debugger..

# It doesn't recognize my motion! #

## Skeleton tracking easily lost ##

  * I noticed that the skeleton tracking could be easily lost if you were using old unstable versions of OpenNI, NITE, and SensorKinect. Recent versions should have been improved a lot.
  * Your surroundings may impact on the ability of skeleton tracking.
    * Baggy cloths could make the skeleton tracking difficult. If so, change your cloths.
    * Objects that reflect IR emitted by Kinect could make the skeleton tracking difficult. For example, I have had a trouble that the carpet around my feet is detected as a part of my body. Changing the position of the objects in your room or changing the angle of Kinect could solve the issue.
  * You may need to have a reasonably high frame rate to get a good tracking. A low frame rate could make it difficult for the software to capture your motion flow smoothly.

## Power does not appear ##

  * The parts of your body should be seen clearly and separate from the camera to get a good detection.
    * For example, keeping your hands too close to your head or hiding your head by your hands could make it difficult to shoot Eye Slugger or Emerium Beam. You could have a better detection by having a little space between your head and your hands.
    * For example, placing your left hand too close to your right elbow could make it difficult to shoot Wide Shot. You could have a better detection by having a little space also.
    * If you face the side of your body to the camera, it could make the pose detection difficult because the other side of your body is hard to be seen from the camera.
  * You need to have a reasonably high frame rate (I assume more than 15 frames/sec) to get a good detection. You need to stay on the same pose for several frames until a power is triggered, and a low frame rate makes it difficult. In any ways, the powers would not look neat at a low frame rate.
  * Try to set the Trigger Happy Mode to Happy or Happier by hitting 't' key. This will make the pose detection easier.
  * It seems that the pure-OpenNI configuration (OpenNI+NITE+SensorKinect) gives you the most stable skeleton tracking. Other configurations (Kinect SDK by itself or Kinect SDK & OpenNI-bridge combination) have jitters that makes the pose detection difficult.

# For Kinect Hackers: Compilation and build #

## How can I get the source code? ##

  * Get Subversion (version control system) first. Google to know where to get and how to install.
    * The master project is now in [Apache](http://subversion.apache.org/) although there are several derived works.
    * If you prefer GUI, the popular [TortoiseSVN](http://tortoisesvn.tigris.org/) is recommended.
  * Follow the instruction in the [checkout](http://code.google.com/p/kinect-ultra/source/checkout) page. "Checkout" from https://kinect-ultra.googlecode.com/svn/trunk
    * To access to a specific branch or tag, replace "trunk" with the path to the branch or the tag.

## How can I build? ##

  * Use VC++ 2010 and open .sln in the build2010/ folder.
  * The build/ folder (next to build2010) contains the build files for VC++ 2008, but it can only build OpenNI version and may be obsolete.

## Does not run on the debugger ##

  * Make sure you set ".." (parent directory) to the working directory in the debugger settings. You will get "File Not Found" error otherwise.
  * You may get "CRT not initialized" error if OpenNI.dll is not built in the debug mode. (for OpenNI)
  * You may get "CRT not initialized" error unless you run VC++ as administrator.