## Download ##

  * Download OpenNI and NITE from the [archive site](http://www.openni.org/openni-sdk/openni-sdk-history-2/). To use PrimeSense sensors (not Kinect), download the sensor driver as well. **Choose 32bit version regardless of whether your platform is 32bit or 64bit.**
  * To use Kinect, download Avin2's SensorKinect driver from [here](https://github.com/avin2/SensorKinect/tree/unstable/Bin) and install it. **Choose 32bit.** If you want to use Kinect SDK on the same machine, consider using mssdk-openni-bridge explained later to avoid the driver conflict.
  * If interested, see WorkingConfigurations for the tested version combinations.

## Using kinect-mssdk-openni-bridge ##

  * There is a problem with SensorKinect: You cannot install SensorKinect driver and Microsoft Kinect SDK at the same time. They conflict.
  * kinect-mssdk-openni-bridge is a replacement of SensorKinect to workaround this issue. This bridge leverages Kinect SDK to access to Kinect rather than conflicting to it.
  * To use this bridge, first download and install Kinect SDK (See [KinectSDK\_Downloads](KinectSDK_Downloads.md)), then download the bridge from the [kinect-mssdk-openni-bridge project site](http://code.google.com/p/kinect-mssdk-openni-bridge/) and follow README to install it.