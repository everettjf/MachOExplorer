echo "begin create moex.tar.gz"
rm -rf ./release-cli
mkdir ./release-cli

mkdir ./release-cli/package
mkdir ./release-cli/package/bin
cp ./moex-cli/cmake-build-release/moex ./release-cli/package/bin/moex

cd ./release-cli/package/
tar -zcvf ../moex.tar.gz .
cd ../../

echo "finish create moex.tar.gz"