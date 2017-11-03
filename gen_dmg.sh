rm ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MOEX.dmg
rm ./release/MachOExplorer.dmg

~/qt/5.9.1/clang_64/bin/macdeployqt ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MachOExplorer.app -dmg
mkdir release
cp ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MachOExplorer.dmg ./release/
