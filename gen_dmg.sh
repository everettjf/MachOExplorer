rm ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MOEX.dmg
rm ./release/MOEX.dmg

~/qt/5.9.1/clang_64/bin/macdeployqt ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MOEX.app -dmg
mkdir release
cp ./build-moex-gui-Desktop_Qt_5_9_1_clang_64bit-Release/MOEX.dmg ./release/
