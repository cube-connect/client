#!/bin/bash

# This script packages an executable and its associated directories into a zip file.
# - `read -p`: Prompts the user with a specified message and stores the input in a variable.
# - `read -e`: Enables readline support, allowing for tab completion and other line editing features.
# - `read -a`: Reads input into an array variable, splitting by whitespace.
# - `shift`: Shifts the positional parameters to the left, allowing access to remaining arguments. Used here to separate the `is_windows` argument from the rest of the directories.

# Function to package the executable and directories
package_executable() {
    local build_dir="$1"
    local exe_name="$2"
    local is_windows="$3"
    shift 3  # Shift arguments to the left, so remaining arguments (directories) can be accessed as "$@"
    local package_dirs=("$@")
    local package_name="${exe_name}_package"
    
    if [[ "$is_windows" == "yes" ]]; then
        package_name+="_windows.zip"
    else
        package_name+="_linux.zip"  # Or another default suffix for non-Windows
    fi

    local output_dir="packaged_executables"

    echo "Packaging $exe_name from $build_dir"

    # Create a temporary directory for packaging
    temp_dir=$(mktemp -d)
    mkdir -p "$temp_dir/package"

    # Copy the executable to the package directory
    if [[ "$is_windows" == "yes" ]]; then
        cp "$build_dir/$exe_name.exe" "$temp_dir/package/"
    else
        cp "$build_dir/$exe_name" "$temp_dir/package/"
    fi

    # Copy additional directories relative to the build directory
    for dir in "${package_dirs[@]}"; do
        if [ -d "$build_dir/$dir" ]; then
            cp -r "$build_dir/$dir" "$temp_dir/package/"
        else
            echo "Warning: Directory $dir does not exist in $build_dir"
        fi
    done

    # Create output directory if it doesn't exist
    mkdir -p "$output_dir"

    # Zip the package
    cd "$temp_dir" || exit
    zip -r "$package_name" package/*

    # Move the zip file to the packaged_executables directory
    mv "$package_name" "$OLDPWD/$output_dir"

    # Cleanup
    cd "$OLDPWD" || exit
    rm -rf "$temp_dir"

    echo "Packaging complete: $output_dir/$package_name"
}

# Enable tab completion for path inputs
read_path() {
    local prompt="$1"
    local var_name="$2"
    read -e -p "$prompt" "$var_name"
}

# Read array of paths with tab completion
read_paths_array() {
    local prompt="$1"
    local var_name="$2"
    echo "$prompt"
    read -e -a "$var_name"
}

# Ask for the build directory and executable name
read_path "Enter the build directory: " build_dir
read -p "Enter the executable name: " exe_name

# Ask if it's a Windows executable
read -p "Are you packaging a Windows executable? (yes/no): " is_windows

# Ask for additional directories to package, relative to the build directory
read_paths_array "Enter any additional directories (relative to the build directory) to include (space-separated, leave blank if none): " dirs

# Run the packaging function
package_executable "$build_dir" "$exe_name" "$is_windows" "${dirs[@]}"
