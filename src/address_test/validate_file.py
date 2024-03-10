def parse_mos_line(line):
    """
    Parse a single line of MOS paper tape format.
    
    Args:
        line (str): A string in the MOS paper tape format.
    
    Returns:
        dict: A dictionary containing the number of data points, address, data points, and checksum.
    """
    # Ensure the line starts with a semicolon and strip it off
    if not line.startswith(';'):
        raise ValueError("Line does not start with a semicolon (;).")
    line = line[1:]
    
    # Extract components based on the format
    num_data_points = int(line[:2], 16)
    address = line[2:6]
    data_points = line[6:-4]
    checksum = line[-4:]
    
    # Split the data points into bytes
    data_points_bytes = [data_points[i:i+2] for i in range(0, len(data_points), 2)]
    
    # Ensure the number of data points matches
    if num_data_points != len(data_points_bytes):
        raise ValueError("Number of data points does not match the length specified.")
    
    return {
        "num_data_points": num_data_points,
        "address": address,
        "data_points": data_points_bytes,
        "checksum": checksum
    }

def parse_file(filename):
    """
    Parse a file containing lines in the MOS paper tape format.
    
    Args:
        filename (str): The path to the file to parse.
    """
    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()  # Remove any leading/trailing whitespace
            if line:  # Ensure the line is not empty
                try:
                    parsed_line = parse_mos_line(line)
                    print(parsed_line)
                except ValueError as e:
                    print(f"Error parsing line: {e}")

# Example usage
filename = "raw_dump.ptp"
parse_file(filename)
