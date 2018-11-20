::rmdir /s /q build_windows

mkdir build_windows
cd build_windows


::C:\Qt\Qt5.11.2\5.11.2\msvc2017_64\bin\qmake.exe ..\src\MachOExplorer.pro
::nmake

cd release
rmdir /s /q dist
mkdir dist
copy MachOExplorer.exe dist\MachOExplorer.exe
cd dist
C:\Qt\Qt5.11.2\5.11.2\msvc2017_64\bin\windeployqt.exe MachOExplorer.exe

cd ..
cd ..
cd ..

mkdir dist
rmdir dist\MachOExplorer_Windows
xcopy build_windows\release\dist dist\MachOExplorer_Windows\ /s /e /c /y /h /r
