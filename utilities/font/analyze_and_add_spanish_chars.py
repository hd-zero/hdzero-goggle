#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import subprocess
import os
import glob

# --- Configuration ---
output_dir = 'out'  # Directory for generated fonts

# Characters to add to the Montserrat range.
additional_characters = [
    # Spanish
    "¡", "¿", "Á", "É", "Í", "Ñ", "Ó", "Ú",
    "á", "é", "í", "ñ", "ó", "ú", "ü",
    # German
    "Ä", "Ö", "Ü", "ß", "ä", "ö", "ü",
    # Polish
    "Ą", "Ć", "Ę", "Ł", "Ń", "Ó", "Ś", "Ź", "Ż",
    "ą", "ć", "ę", "ł", "ń", "ó", "ś", "ź", "ż",
]

def extract_simplified_chinese_unicode():
    """Extract Chinese characters from zh_hans.ini."""
    input_file_path = "../../mkapp/app/language/zh_hans.ini"
    range_str = ""
    char_pattern = re.compile(r'[\u4e00-\u9fff]')
    unique_chars = set()

    try:
        with open(input_file_path, "r", encoding="utf-8") as file:
            for line in file:
                for char in line:
                    if char_pattern.match(char):
                        unique_chars.add(char)

        for char in sorted(unique_chars):
            range_str += f"{ord(char)},"

        return range_str[:-1]
    except FileNotFoundError:
        print(f"Warning: {input_file_path} not found. Using empty Chinese range.")
        return ""

def update_range_with_extra_characters(original_range):
    """Add Spanish, German, and Polish characters to an existing font range."""
    if not original_range:
        return original_range
    
    # Expand the original range into code points.
    existing_points = set()
    for part in original_range.split(','):
        if '-' in part:
            start, end = map(int, part.split('-'))
            existing_points.update(range(start, end + 1))
        else:
            existing_points.add(int(part))
    
    # Add the listed characters; shared letters only need one code point.
    new_points = existing_points.union(ord(char) for char in additional_characters)
    
    # Compress consecutive code points back into ranges.
    sorted_points = sorted(new_points)
    ranges = []
    i = 0
    while i < len(sorted_points):
        start = sorted_points[i]
        while i + 1 < len(sorted_points) and sorted_points[i + 1] == sorted_points[i] + 1:
            i += 1
        end = sorted_points[i]
        
        if start == end:
            ranges.append(str(start))
        else:
            ranges.append(f"{start}-{end}")
        i += 1
    
    return ','.join(ranges)

def process_font_file(input_file):
    """Regenerate one font file with the additional characters."""
    output_file = os.path.join(output_dir, os.path.basename(input_file))
    
    print(f"\nProcessing: {input_file}")
    
    try:
        # Read the original conversion command from the C file.
        with open(input_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Find the Opts line containing that command.
        opts_match = re.search(r'\*\s*Opts:\s*(.*)', content)
        if not opts_match:
            print(f"  Warning: No 'Opts:' line found in {input_file}")
            return False
        
        original_command = opts_match.group(1).strip()
        
        # Find the current Montserrat character range.
        montserrat_range_match = re.search(r'--font\s+Montserrat-Medium\.ttf\s+--range\s+([\d,-]+)', original_command)
        if not montserrat_range_match:
            print(f"  Warning: No Montserrat range found in {input_file}")
            return False
        
        original_range = montserrat_range_match.group(1)
        print(f"  Original range: {original_range}")
        
        # Add Spanish, German, and Polish characters.
        updated_range = update_range_with_extra_characters(original_range)
        print(f"  Updated range: {updated_range}")
        
        # Keep the other font conversion options unchanged.
        new_command = original_command.replace(
            f"--font Montserrat-Medium.ttf --range {original_range}",
            f"--font Montserrat-Medium.ttf --range {updated_range}"
        )
        
        # Write the regenerated font to the output directory.
        basename = os.path.basename(input_file)
        new_command = new_command.replace(
            f"-o out/{basename}",
            f"-o {output_file}"
        )
        
        print("  Generating font...")
        # Run the converter without a shell.
        cmd_parts = ['lv_font_conv'] + new_command.split()
        result = subprocess.run(cmd_parts, capture_output=True, text=True)
        
        if result.returncode == 0:
            print(f"  ✓ Font generated: {output_file}")
            return True
        else:
            print(f"  ✗ Font generation failed: {result.stderr}")
            if result.stdout:
                print(f"    Output: {result.stdout}")
            return False
            
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def main():
    # Use paths relative to this script, regardless of the shell's current directory.
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Create the output directory if needed.
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Created output directory: {output_dir}")
    
    # Find every generated Montserrat font file.
    font_files = sorted(glob.glob("out/lv_font_montserrat_*.c"))
    
    if not font_files:
        print("No lv_font_montserrat_*.c files found in out. Run generate_font_lib.py first.")
        return
    
    print(f"Found {len(font_files)} font files:")
    for f in font_files:
        print(f"  - {f}")
    
    # Regenerate every font file.
    success_count = 0
    failed_count = 0
    
    for font_file in font_files:
        if process_font_file(font_file):
            success_count += 1
        else:
            failed_count += 1
    
    # Summary
    print("\n" + "="*50)
    print("Processing complete:")
    print(f"  ✓ Successful: {success_count}")
    print(f"  ✗ Failed: {failed_count}")
    print(f"  Total: {len(font_files)}")
    print("="*50)
    
    if success_count > 0:
        print(f"\nUpdated fonts are in: {output_dir}/")
        print("Copy the files to the project's font directory:")
        print(f"  cp {output_dir}/* ../../lib/lvgl/lvgl/src/font/")

if __name__ == "__main__":
    main()
