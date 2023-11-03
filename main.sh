#!/usr/bin/env bash
src="mmap-access-pattern-openmp"
out="$HOME/Logs/$src.log"
ulimit -s unlimited
printf "" > "$out"

# Download program
if [[ "$DOWNLOAD" != "0" ]]; then
  rm -rf $src
  git clone https://github.com/puzzlef/$src
fi
cd $src

# Fixed config
: "${MAX_THREADS:=64}"
# Define macros (dont forget to add here)
DEFINES=(""
"-DMAX_THREADS=$MAX_THREADS"
)

# Build and run
g++ ${DEFINES[*]} -std=c++17 -O3 -fopenmp main.cxx

perform-all() {
par="$1"    # 0=serial, 1=parallel
adv="$2"    # 0=none, 1=early madvise
block="$3"  # block size
mode="$4"   # 0=none, 1=madvise, 2=mmap
stdbuf --output=L ./a.out ~/Data/indochina-2004.mtx  $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/uk-2002.mtx         $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/arabic-2005.mtx     $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/uk-2005.mtx         $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/webbase-2001.mtx    $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/it-2004.mtx         $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/sk-2005.mtx         $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/com-LiveJournal.mtx $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/com-Orkut.mtx       $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/asia_osm.mtx        $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/europe_osm.mtx      $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/kmer_A2a.mtx        $par $adv $block $mode 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/Data/kmer_V1r.mtx        $par $adv $block $mode 2>&1 | tee -a "$out"
}

perform-all 1

# Signal completion
curl -X POST "https://maker.ifttt.com/trigger/puzzlef/with/key/${IFTTT_KEY}?value1=$src$1"
