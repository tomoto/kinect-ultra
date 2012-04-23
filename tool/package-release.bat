@rem Note: (1) Set VER to release version. (2) Install ActivePerl.

setlocal
set VER=1.0a

set TARGET=release\kinect-ultra_%VER%_for_KinectSDK
rmdir /s /q %TARGET%
mkdir %TARGET%
xcopy ..\data %TARGET%\data\ /e
copy ..\build2010\MSSDK_Release\kinect-ultra.exe %TARGET%\
copy ..\build2010\MSSDK_Release\opencv_*.dll %TARGET%
copy ..\LICENSE.TXT %TARGET%
copy ..\README_*.TXT %TARGET%
copy ..\HISTORY_*.TXT %TARGET%
perl zip.pl %TARGET%

set TARGET=release\kinect-ultra_%VER%_for_OpenNI
rmdir /s /q %TARGET%
mkdir %TARGET%
xcopy ..\data %TARGET%\data\ /e
copy ..\build2010\OpenNI_Release\kinect-ultra.exe %TARGET%\
copy ..\build2010\OpenNI_Release\opencv_*.dll %TARGET%
copy ..\LICENSE.TXT %TARGET%
copy ..\README_*.TXT %TARGET%
copy ..\HISTORY_*.TXT %TARGET%
perl zip.pl %TARGET%

