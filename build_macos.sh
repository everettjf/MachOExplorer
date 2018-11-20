
rm -rf build_macos

mkdir build_macos
cd build_macos

# compile
/opt/qt/5.11.2/clang_64/bin/qmake ../src/MachOExplorer.pro
make

# create temp dmg
/opt/qt/5.11.2/clang_64/bin/macdeployqt ./MachOExplorer.app -dmg


# repack dmg
rm -rf dist
mkdir dist
hdiutil detach /Volumes/MachOExplorer
hdiutil attach MachOExplorer.dmg
cp -R /Volumes/MachOExplorer/MachOExplorer.app ./dist/MachOExplorer.app
hdiutil detach /Volumes/MachOExplorer

# remove 

cd dist
cp ../../res/appdmg.json ./
cp ../../res/background.png ./
appdmg appdmg.json ./MachOExplorer.dmg

cd ..
cd .. 

mkdir dist
rm dist/MachOExplorer.dmg
cp -f build_macos/dist/MachOExplorer.dmg dist/

echo "---------"
echo "done :)"
echo "---------"
