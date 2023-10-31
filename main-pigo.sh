#!/usr/bin/env bash
src="test-graph-frameworks"
out="$HOME/Logs/$src$1.log"
ulimit -s unlimited
printf "" > "$out"


# Download program, PIGO
if [[ "$DOWNLOAD" != "0" ]]; then
  rm -rf $src
  rm -rf PIGO
  git clone https://github.com/puzzlef/$src
  git clone https://github.com/GT-TDAlab/PIGO
  cd $src
fi

# Copy pigo.hpp to external include directory
mkdir -p ext
rm -f ext/pigo.hpp
cp ../PIGO/release/pigo.hpp ext/pigo.hpp

# Fixed config
: "${MAX_THREADS:=64}"
: "${REPEAT_METHOD:=5}"
# Define macros (dont forget to add here)
DEFINES=(""
"-DMAX_THREADS=$MAX_THREADS"
"-DREPEAT_METHOD=$REPEAT_METHOD"
)

# Run
g++ ${DEFINES[*]} -std=c++17 -O3 -fopenmp main-pigo.cxx
# stdbuf --output=L ./a.out ~/Data/web-Stanford.mtx   0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/indochina-2004.mtx  0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/uk-2002.mtx         0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/arabic-2005.mtx     0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/uk-2005.mtx         0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/webbase-2001.mtx    0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/it-2004.mtx         0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/sk-2005.mtx         0 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/com-LiveJournal.mtx 1 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/com-Orkut.mtx       1 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/asia_osm.mtx        1 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/europe_osm.mtx      1 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/kmer_A2a.mtx        1 0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/kmer_V1r.mtx        1 0 2>&1 | tee -a "$out"