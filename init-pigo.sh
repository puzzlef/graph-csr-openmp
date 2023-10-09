#!/usr/bin/env bash
# Download PIGO
rm -rf tmp/PIGO
git clone https://github.com/GT-TDAlab/PIGO tmp/PIGO

# Copy pigo.hpp to external include directory
mkdir -p ext
rm -f ext/pigo.hpp
cp tmp/PIGO/release/pigo.hpp ext/pigo.hpp

# Clean up PIGO
rm -rf tmp/PIGO
