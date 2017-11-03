rm ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MachOExplorer.dmg
rm ./release/MachOExplorer.dmg

mkdir release
cd ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/
~/qt/5.9.1/clang_64/bin/macdeployqt ./MachOExplorer.app -dmg
cp ./MachOExplorer.dmg ../release/MachOExplorer.dmg
cd ../

