rm -rf ./release
mkdir ./release

echo 'begin generate dmg'

rm ./build-MachOExplorer-Desktop_Qt_5_10_1_clang_64bit-Release/MachOExplorer.dmg

cd ./build-MachOExplorer-Desktop_Qt_5_10_1_clang_64bit-Release/
~/qt/5.10.1/clang_64/bin/macdeployqt ./MachOExplorer.app -dmg
cp ./MachOExplorer.dmg ../release/MachOExplorer.dmg
cd ../

echo 'complete generate dmg'

