#!/bin/bash

# ========================
# Configuration
# ========================

# Array of absolute directories to process
TargetDirs=(
    "C:\Users\Admin\Desktop\RandoGen\app\src\main\cpp"
	"C:\Users\Admin\Desktop\RandoGen\app\src\main\java\com\example\randogen"
)

# Array of folder names to exclude (relative inside each target dir)
ExcludeDirs=("includes" "BlueprintBlockBuilder")

# Array of file names to exclude (basename only)
ExcludeFiles=("TestUtilities.h" "CMakeLists.txt")

# Array of file extensions to exclude (suffix match)
ExcludeExtensions=(".test" ".test.cpp" ".mk" ".txt" ".webp")

# ========================
# Validate targets
# ========================

for Dir in "${TargetDirs[@]}"; do
    if [ ! -d "$Dir" ]; then
        echo "Error: '$Dir' is not a valid directory."
        exit 1
    fi
done

# ========================
# Output file
# ========================

OutputFile="MergedOutput.cpp"
> "$OutputFile"

echo "Merging files from:"
for Dir in "${TargetDirs[@]}"; do
    echo "  - $Dir"
done

echo "Excluding folders: ${ExcludeDirs[*]}"
echo "Excluding files: ${ExcludeFiles[*]}"
echo "Excluding extensions: ${ExcludeExtensions[*]}"
echo "Output: $OutputFile"

# ========================
# Process each directory
# ========================

for TargetDir in "${TargetDirs[@]}"; do

    while IFS= read -r FilePath; do

        RelPath="${FilePath#$TargetDir/}"
        FileName="$(basename "$FilePath")"

        # Exclude by file name
        for ExclFile in "${ExcludeFiles[@]}"; do
            if [[ "$FileName" == "$ExclFile" ]]; then
                echo "Skipping file: $FileName"
                continue 2
            fi
        done

        # Exclude by extension
        for Ext in "${ExcludeExtensions[@]}"; do
            if [[ "$FileName" == *"$Ext" ]]; then
                echo "Skipping file by extension: $FileName"
                continue 2
            fi
        done

        echo "// ===== File: $RelPath =====" >> "$OutputFile"
        cat "$FilePath" >> "$OutputFile"
        echo "" >> "$OutputFile"

    done < <(
        find "$TargetDir" \
        $(for ExclDir in "${ExcludeDirs[@]}"; do
            echo -path "$TargetDir/$ExclDir" -o -path "$TargetDir/$ExclDir/*" -o
          done) \
        -false -o -type f -print
    )

done

echo "Done."