#!/usr/bin/env bash
src="graph-csr-openmp"
out="$HOME/Logs/$src$1.log"
ulimit -s unlimited
printf "" > "$out"

# Download program
if [[ "$DOWNLOAD" != "0" ]]; then
  rm -rf $src
  git clone https://github.com/puzzlef/$src
fi
cd $src

# Fixed config
: "${KEY_TYPE:=uint32_t}"
: "${EDGE_VALUE_TYPE:=float}"
: "${MAX_THREADS:=1}"

# Perform necessary steps
perform-all() {
# Define macros (dont forget to add here)
DEFINES=(""
"-DKEY_TYPE=$KEY_TYPE"
"-DEDGE_VALUE_TYPE=$EDGE_VALUE_TYPE"
"-DMAX_THREADS=$1"
)
# Run
g++ ${DEFINES[*]} -std=c++17 -O3 -march=native -fopenmp main.cxx
# stdbuf --output=L ./a.out ~/Data/soc-Epinions1.mtx   2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/indochina-2004.mtx  2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/uk-2002.mtx         2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/arabic-2005.mtx     2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/uk-2005.mtx         2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/webbase-2001.mtx    2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/it-2004.mtx         2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/sk-2005.mtx         2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/com-LiveJournal.mtx 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/com-Orkut.mtx       2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/asia_osm.mtx        2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/europe_osm.mtx      2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/kmer_A2a.mtx        2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/kmer_V1r.mtx        2>&1 | tee -a "$out"
}

for i in {1..5}; do
  perform-all 1
  perform-all 64
done

# Signal completion
curl -X POST "https://maker.ifttt.com/trigger/puzzlef/with/key/${IFTTT_KEY}?value1=$src$1"
