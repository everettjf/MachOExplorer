rm -rf ./release
mkdir ./release

sh dist_cli.sh
sh dist_dmg.sh

cp ./release-gui/MachOExplorer.dmg ./release/
cp ./release-cli/moex.tar.gz ./release/
