import urllib.request
import re

URL = "https://www.unicode.org/Public/UCD/latest/ucd/DerivedCoreProperties.txt"

def parse_ranges(data, property_name):
    ranges = []
    # Regex to capture: "0041..005A" or "00AA" followed by property name
    # Group 1: Start Hex
    # Group 2: End Hex (optional)
    pattern = re.compile(f"^([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*{property_name}")

    for line in data.splitlines():
        # Remove comments after #
        clean_line = line.split('#')[0].strip()
        match = pattern.match(clean_line)
        
        if match:
            start = int(match.group(1), 16)
            if match.group(2):
                end = int(match.group(2), 16)
            else:
                end = start # Single char range
            
            ranges.append((start, end))
            
    return ranges

def merge_ranges(ranges):
    if not ranges:
        return []
    
    ranges.sort()
    merged = [ranges[0]]
    
    for current_start, current_end in ranges[1:]:
        last_start, last_end = merged[-1]
        
        # If the current range starts exactly where the last one ended + 1, merge them
        if current_start == last_end + 1:
            merged[-1] = (last_start, current_end)
        else:
            merged.append((current_start, current_end))
            
    return merged

def print_c_array(name, ranges):
    print(f"// Generated from Unicode DerivedCoreProperties.txt")
    print(f"// Property: {name}")
    print(f"static const UnicodeRange {name.lower()}_ranges[] = {{")
    
    # Print in grid for readability
    for i, (start, end) in enumerate(ranges):
        # Format: {0x0041, 0x005A}, 
        s_hex = f"0x{start:04X}"
        e_hex = f"0x{end:04X}"
        entry = f"    {{{s_hex}, {e_hex}}},"
        
        # Add comment for clarity on ASCII ranges
        if start <= 0x7F:
            entry += f" // Basic Latin"
            
        print(entry)

    print("};")
    print(f"static const size_t {name.lower()}_count = {len(ranges)};\n")

def main():
    print(f"// Downloading data from {URL} ...")
    with urllib.request.urlopen(URL) as response:
        data = response.read().decode('utf-8')

    print("#include <stdint.h>")
    print("#include <stddef.h>")
    print("\ntypedef struct { uint32_t start; uint32_t end; } UnicodeRange;\n")

    # 1. XID_Start
    raw_start = parse_ranges(data, "XID_Start")
    merged_start = merge_ranges(raw_start)
    print_c_array("XID_Start", merged_start)

    # 2. XID_Continue
    raw_continue = parse_ranges(data, "XID_Continue")
    merged_continue = merge_ranges(raw_continue)
    print_c_array("XID_Continue", merged_continue)

if __name__ == "__main__":
    main()