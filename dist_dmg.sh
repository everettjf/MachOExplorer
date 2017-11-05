echo 'begin generate dmg'

rm -rf ./release-gui
rm ./build-MachOExplorer-Desktop_Qt_5_9_1_clang_64bit-Release/MachOExplorer.dmg

mkdir ./release-gui
cd ./build-MachOExplorer-Desktop_Qt_5_9_1_clang_64bit-Release/

~/qt/5.9.1/clang_64/bin/macdeployqt ./MachOExplorer.app -dmg

cp ./MachOExplorer.dmg ../release-gui/MachOExplorer.dmg
cd ../

echo 'complete generate dmg'