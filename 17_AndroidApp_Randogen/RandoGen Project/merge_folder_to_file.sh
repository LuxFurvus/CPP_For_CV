#!/bin/bash

# ========================
# Configuration
# ========================


TargetDirs=(
    "D:\ForCPP\CPP_For_CV\17_AndroidApp_Randogen\RandoGen Project\app\src\main\java\com\example\randogen\Common"
)

TargetFiles=(
)

ExcludeDirs=("includes" "BlueprintBlockBuilder")

ExcludeFiles=("TestUtilities.h" "CMakeLists.txt")

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

for File in "${TargetFiles[@]}"; do
    if [ ! -f "$File" ]; then
        echo "Error: '$File' is not a valid file."
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

if [ "${#TargetFiles[@]}" -gt 0 ]; then
    echo "Always include files:"
    for File in "${TargetFiles[@]}"; do
        echo "  - $File"
    done
fi

echo "Excluding folders: ${ExcludeDirs[*]}"
echo "Excluding files: ${ExcludeFiles[*]}"
echo "Excluding extensions: ${ExcludeExtensions[*]}"
echo "Output: $OutputFile"

# ========================
# Helpers
# ========================

declare -A AddedFiles

AppendFile() {
    local FilePath="$1"
    local DisplayPath="$2"

    if [ -n "${AddedFiles["$FilePath"]+x}" ]; then
        return
    fi

    echo "// ===== File: $DisplayPath =====" >> "$OutputFile"
    cat "$FilePath" >> "$OutputFile"
    echo "" >> "$OutputFile"

    AddedFiles["$FilePath"]=1
}

ShouldExcludeFile() {
    local FilePath="$1"
    local FileName
    FileName="$(basename "$FilePath")"

    for ExclFile in "${ExcludeFiles[@]}"; do
        if [[ "$FileName" == "$ExclFile" ]]; then
            return 0
        fi
    done

    for Ext in "${ExcludeExtensions[@]}"; do
        if [[ "$FileName" == *"$Ext" ]]; then
            return 0
        fi
    done

    return 1
}

FindFilesInDir() {
    local TargetDir="$1"
    local PruneExpr=()

    for ExclDir in "${ExcludeDirs[@]}"; do
        PruneExpr+=( -name "$ExclDir" -o )
    done

    if [ "${#PruneExpr[@]}" -gt 0 ]; then
        unset "PruneExpr[$((${#PruneExpr[@]} - 1))]"

        find "$TargetDir" \
            -type d \( "${PruneExpr[@]}" \) -prune \
            -o -type f -print0
    else
        find "$TargetDir" -type f -print0
    fi
}

# ========================
# Process each directory
# ========================

for TargetDir in "${TargetDirs[@]}"; do
    while IFS= read -r -d '' FilePath; do
        RelPath="${FilePath#"$TargetDir"/}"

        if ShouldExcludeFile "$FilePath"; then
            echo "Skipping file: $FilePath"
            continue
        fi

        AppendFile "$FilePath" "$RelPath"
    done < <(FindFilesInDir "$TargetDir")
done

# ========================
# Include forced files
# ========================

for FilePath in "${TargetFiles[@]}"; do
    echo "Force including file: $FilePath"
    AppendFile "$FilePath" "$FilePath"
done

echo "Done."