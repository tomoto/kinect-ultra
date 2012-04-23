@rem Note: (1) Set VER to release version. (2) Install ActivePerl.

setlocal
set VER=1.0a
set TARGET=release\kinect-ultra_%VER%
rmdir /s /q %TARGET%
mkdir %TARGET%
xcopy ..\data %TARGET%\data\ /e
copy ..\build\release\kinect-ultra.exe %TARGET%\
copy ..\build\release\cxcore210.dll %TARGET%
copy ..\build\release\highgui210.dll %TARGET%
copy ..\LICENSE.TXT %TARGET%
copy ..\README_*.TXT %TARGET%
copy ..\HISTORY_*.TXT %TARGET%
perl zip.pl %TARGET%
