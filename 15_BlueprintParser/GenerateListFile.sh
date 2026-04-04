#!/bin/bash

# ==========================
# Configuration
# ==========================

# Set the folder to be processed here
TargetFolder="./src"  # Change this path as needed

# ==========================
# Validation
# ==========================

if [[ ! -d "$TargetFolder" ]]; then
    echo "Error: Target folder '$TargetFolder' does not exist or is not a directory."
    exit 1
fi

# ==========================
# Determine Output File
# ==========================

# Get the base name of the folder (no path)
FolderName="$(basename "$TargetFolder")"
OutputFile="${FolderName}.list"

# ==========================
# Generate Relative Path List
# ==========================

# Navigate to the target folder's parent
ParentDir="$(dirname "$TargetFolder")"
cd "$ParentDir" || exit 1

# Write all relative paths into the output file
find "$FolderName" > "$OutputFile"

# ==========================
# Completion Message
# ==========================

echo "List created: $(realpath "$OutputFile")"
