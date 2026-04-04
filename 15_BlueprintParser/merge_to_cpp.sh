#!/bin/bash

# ========================
# Configuration
# ========================
TargetDir="OLD"

# Array of folder names to exclude (relative to TargetDir)
ExcludeDirs=("FieldParser" "BlueprintBlockBuilder")

# Array of file names to exclude (basename only)
ExcludeFiles=("TestUtilities.h" "COMMANDS.sh")

# Array of file extensions to exclude (suffix match)
ExcludeExtentions=(".test" ".test.cpp" ".mk" ".h")

# ========================
# Validate target
# ========================
if [ ! -d "$TargetDir" ]; then
    echo "Error: '$TargetDir' is not a valid directory."
    exit 1
fi

# ========================
# Derive output file name
# ========================
FolderName="$(basename "$TargetDir")"
OutputFile="${FolderName}.cpp"

> "$OutputFile"

echo "Merging all files from: $TargetDir"
echo "Excluding folders: ${ExcludeDirs[*]}"
echo "Excluding files: ${ExcludeFiles[*]}"
echo "Excluding extensions: ${ExcludeExtentions[*]}"
echo "Output: $OutputFile"

# ========================
# Build dynamic -prune expression for find
# ========================
ExcludeFindExpr=""
for ExclDir in "${ExcludeDirs[@]}"; do
    ExcludeFindExpr+=" -path \"$TargetDir/$ExclDir\" -o"
    ExcludeFindExpr+=" -path \"$TargetDir/$ExclDir/*\" -o"
done
ExcludeFindExpr="${ExcludeFindExpr::-2}"  # Remove trailing '-o'

# ========================
# Find and process files
# ========================
while IFS= read -r FilePath; do
    RelPath="${FilePath#$TargetDir/}"         # Strip leading target dir
    FileName="$(basename "$FilePath")"

    # Check file name exclusion
    for ExclFile in "${ExcludeFiles[@]}"; do
        if [[ "$FileName" == "$ExclFile" ]]; then
            echo "Skipping file: $FileName"
            continue 2
        fi
    done

    # Check extension exclusion
    for Ext in "${ExcludeExtentions[@]}"; do
        if [[ "$FileName" == *"$Ext" ]]; then
            echo "Skipping file by extension: $FileName"
            continue 2
        fi
    done

    echo "// ===== File: $RelPath =====" >> "$OutputFile"
    cat "$FilePath" >> "$OutputFile"
    echo -e "\n" >> "$OutputFile"
done < <(eval "find \"$TargetDir\" \\( $ExcludeFindExpr \\) -prune -o -type f -print")

echo "Done."

# # ========================
# # Post-processing: Remove lines from markers
# # ========================

# # Temporary output file after filtering
# CleanedOutput="${OutputFile%.cpp}_filtered.cpp"

# # Call the cleaning script and overwrite the original
# ./remove_marked_lines.sh "$OutputFile" > "$CleanedOutput" && mv "$CleanedOutput" "$OutputFile"

# echo "Post-processed with remove_marked_lines.sh"
