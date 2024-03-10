def parse_mos_line(line):
    """
    Parse a single line of MOS paper tape format.
    """
    if not line.startswith(';'):
        raise ValueError("Line does not start with a semicolon (;).")
    line = line[1:]
    
    num_data_points = int(line[:2], 16)
    address = int(line[2:6], 16)
    data_points = line[6:-4]
    checksum = line[-4:]
    
    data_points_bytes = [int(data_points[i:i+2], 16) for i in range(0, len(data_points), 2)]
    
    if num_data_points != len(data_points_bytes):
        raise ValueError("Number of data points does not match the length specified.")
    
    return {
        "num_data_points": num_data_points,
        "address": address,
        "data_points": data_points_bytes,
        "checksum": checksum
    }

def parse_file_and_fill_array(filename):
    """
    Parse a file and fill an array based on parsed addresses and values.
    """
    memory = {}  # Using a dictionary to handle sparse addresses efficiently
    highest_address = 0

    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            if line:
                try:
                    parsed_line = parse_mos_line(line)
                    address = parsed_line["address"]
                    for offset, value in enumerate(parsed_line["data_points"]):
                        memory[address + offset] = value
                        highest_address = max(highest_address, address + offset)
                except ValueError as e:
                    print(f"Error parsing line: {e}")
    
    # Now convert the dictionary to a list for contiguous memory representation
    # Initialize the array with zeros up to the highest address encountered
    memory_array = [0] * (highest_address + 1)
    for address, value in memory.items():
        memory_array[address] = value
    
    return memory_array


def compare_memory_to_high_byte(memory_array):
    """
    Compares the value at each memory location in the range 0x2000 to 0xFFFF
    to the high byte of its address.
    
    Args:
        memory_array (list): The memory array containing the values at each address.
        
    Returns:
        list: A list of tuples (address, value) for addresses where the value
              does not match the high byte of the address.
    """
    mismatches = []
    # Ensure the memory array can accommodate the highest address
    if len(memory_array) < 0x10000:
        # Extend the memory_array to include addresses up to 0xFFFF
        memory_array.extend([0] * (0x10000 - len(memory_array)))

    for address in range(0x2000, 0x10000):
        high_byte = (address >> 8) & 0xFF
        value = memory_array[address]
        if value != high_byte:
            mismatches.append((hex(address), value))
    
    return mismatches

# Assuming memory_array is already filled from the previous step


# Example usage
filename = "raw_dump.ptp"
memory_array = parse_file_and_fill_array(filename)

mismatches = compare_memory_to_high_byte(memory_array)
if mismatches:
    print("Found mismatches at the following addresses:")
    for address, value in mismatches:
        print(f"Address {address}: Value {value} does not match its high byte.")
else:
    print("All values match their high byte in the address range 0x2000 to 0xFFFF.")
