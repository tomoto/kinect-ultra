kinect-ultra V0.1f-rc
Author: Tomoto S. Washio
Release Date: mm/dd/yyyy
Project URL: http://code.google.com/p/kinect-ultra/

1. Prerequisitive

(1) You already have Kinect, right?
(2) This product is supposed to run on sevral Windows platforms although
    it is only tested on Windows XP Pro SP3 at this moment.
(3) Your computer should have sufficiently fast CPU and GPU.
    - The minimum requirement is unknown
    - The GPU must support OpenGL 2.x or higher, programmable shaders,
      and VAOs at least.
    - Core i7 920 + GeForce GTX 285 is sufficient at least.
    See http://code.google.com/p/kinect-ultra/wiki/WorkingConfigurations
    for the list of the hardware/software configurations this program is
    reported to work.

2. Installation

(1) If you have not yet done, download OpenNI and install it.
    http://www.openni.org/downloadfiles/2-openni-binaries

(2) If you have not yet done, download NITE and install it.
    http://www.openni.org/downloadfiles/12-openni-compliant-middleware-binaries

    Use the following string as the license key during installation:
    0KOIk2JeIBYClPWVnMoRKn5cdY4=

(3) If you have not yet done, download OpenNI-compatible Kinect driver
    and install it.
    https://github.com/avin2/SensorKinect/tree/master/Bin

    If you already have OpenKinect or other drivers installed, you will
    need to switch those drivers.

    The recommended versions of above software components are as follows
    as to today's best knowledge. It may change, so check out the project
    site for the up-to-date information.
    - OpenNI unstable v1.1.0.41: http://www.openni.org/downloadfiles/openni-binaries/latest-unstable/108-openni-unstable-build-for-windows-x86-32-bit-v1-1-0/download
    - NITE unstable v1.3.1.5: http://www.openni.org/downloadfiles/openni-compliant-middleware-binaries/latest-unstable/115-primesense-nite-unstable-build-for-windows-x86-32-bit-v1-3-1/download
    - SensorKinect driver for unstable OpenNI 5.0.1: https://github.com/avin2/SensorKinect/blob/unstable/Bin/SensorKinect-Win-OpenSource32-5.0.1.msi

(4) Unpack this archive anywhere you like.

3. How to Use

(1) Just run kinect-ultra.exe. Set the working directory to the same
    location as the executable.

(2) See the demo at:
    - English
      http://www.youtube.com/watch?v=RUG-Uvq-J-w
  Å@Å@http://www.youtube.com/watch?v=Uuq9SCL_LXY
    - Japanese
      http://www.youtube.com/watch?v=eCbURRDUUdI
      http://www.youtube.com/watch?v=kxvn98lqr5Y

(3) If you have any issues...
    - See FAQ at http://code.google.com/p/kinect-ultra/wiki/FAQ_en
    - Report issues at http://code.google.com/p/kinect-ultra/issues

4. License

See LICENSE.TXT.

5. Build

If you want to build this program yourself or modify, go to the project
page, download the source code, and open build/kinect-ultra.sln with
Visual C++ 2008. To use debugger, set the working directory to ".."
in the project's preference.

Enjoy!
Tomoto
