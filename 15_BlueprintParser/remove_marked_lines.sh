#!/bin/bash

# ======================
# Configuration
# ======================

# Array of marker prefixes
Markers=("#include" "#pragma once" "//")

# File to process (or read from stdin)
InputFile="$1"

# ======================
# Read input
# ======================

if [ -n "$InputFile" ]; then
    InputStream=$(cat "$InputFile")
else
    InputStream=$(cat)
fi

# ======================
# Build regex pattern
# ======================

# Escape and join with '|'
Escaped=()
for Marker in "${Markers[@]}"; do
    Escaped+=( "$(printf '%s\n' "$Marker" | sed -e 's/[]\/$*.^|[]/\\&/g')" )
done

RegexPattern="^([[:space:]]*)($(IFS="|"; echo "${Escaped[*]}"))"

# ======================
# Filter lines
# ======================

echo "$InputStream" | awk -v pattern="$RegexPattern" '
    $0 ~ pattern { next }
    { print }
'
