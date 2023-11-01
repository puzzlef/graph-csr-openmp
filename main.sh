#!/usr/bin/env bash
src="test-graph-frameworks"

# Download program
if [[ "$DOWNLOAD" != "0" ]]; then
  rm -rf $src
  git clone https://github.com/puzzlef/$src
fi
cd $src

# Download scripts to test graph frameworks
wget -O main-pigo.sh     https://github.com/wolfram77/GT-TDAlab--PIGO/raw/master/main.sh
wget -O main-cuhornet.sh https://github.com/wolfram77/rapidsai--cuhornet/raw/master/main.sh
wget -O main-graphone.sh https://github.com/wolfram77/the-data-lab--GraphOne/raw/master/main.sh
wget -O main-gunrock.sh  https://github.com/wolfram77/gunrock--gunrock/raw/master/main.sh
chmod +x main-pigo.sh
chmod +x main-cuhornet.sh
chmod +x main-graphone.sh
chmod +x main-gunrock.sh

# Test each graph framework
./main-pigo.sh
./main-cuhornet.sh
./main-graphone.sh
./main-gunrock.sh
