#!/bin/bash

modify_file() {
    local file_name="$1"
    local parameter_name="$2"
    local new_parameter="$3"

    # Check if file exists
    if [ ! -f "$file_name" ]; then
        echo "File $file_name not found!"
        exit 1
    fi

    # Read file line by line, skipping first two lines
    tail -n +3 "$file_name" | while IFS= read -r line; do
        # Cut off first 16 characters
        rest_of_line="${line:16}"
        
        # Check if the rest of the line matches the parameter name format
        if [[ $rest_of_line =~ ^[[:space:]]*$parameter_name ]]; then
            # Format the new parameter with 16 spaces left aligned
            new_parameter_formatted=$(printf "%-16s" "$new_parameter")

            # Prepend the new parameter to the beginning of the line
            modified_line="$new_parameter_formatted$rest_of_line"
            
            # Replace the line in the file
            sed -i "s|^.*$line$|$modified_line|g" "$file_name"
        fi
    done
}

# Usage: modify_file "file_name" "parameter_name" "new_parameter"
modify_file "hpl_test.dat" "PFACTs" "3"
