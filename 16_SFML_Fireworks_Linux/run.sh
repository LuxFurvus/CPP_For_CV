#!/usr/bin/env bash
set -e

clear
echo -e "\033[1;35m\n[*] Stage 1: Configuring CMake...\n\033[0m"
START=$(date +%s.%N)
cmake -S . -B build
END=$(date +%s.%N)
printf "\033[1;36m[✓] Configure finished in %.4fs\033[0m\n" "$(echo "$END - $START" | bc)"

echo -e "\033[1;35m\n[*] Stage 2: Building (Unity Build)...\n\033[0m"
START=$(date +%s.%N)
cmake --build build
END=$(date +%s.%N)
printf "\033[1;36m[✓] Build finished in %.4fs\033[0m\n" "$(echo "$END - $START" | bc)"

echo -e "\033[1;35m\n[*] Stage 3: Running binary...\n\033[0m"
START=$(date +%s.%N)
ASAN_OPTIONS=detect_leaks=1 \
LSAN_OPTIONS="suppressions=./lsan_suppressions.txt:print_suppressions=1" \
./build/FireWorksApp
END=$(date +%s.%N)
printf "\033[1;36m[✓] Execution finished in %.4fs\033[0m\n" "$(echo "$END - $START" | bc)"

echo -e "\033[1;35m\n[*] Stage 4: Cleanup...\n\033[0m"
rm -rf build
echo -e "\033[1;36m[✓] Done.\033[0m"
