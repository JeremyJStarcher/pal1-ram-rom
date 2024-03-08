def process_line(hex_string):
    # Check if the line starts with a semicolon and remove it
    if hex_string.startswith(';'):
        hex_string = hex_string[1:]
        
    # Extract the count
    count = int(hex_string[0:2], 16)
    # Extract the starting number
    starting_number = int(hex_string[2:6], 16)
    # Calculate the length of the values section
    values_length = count * 2
    # Extract the values
    values_section = hex_string[6:6+values_length]
    # Split the values section into an array of values
    values_array = [values_section[i:i+2] for i in range(0, len(values_section), 2)]
    return starting_number, values_array

def process_file(input_file, output_file):
    with open(input_file, 'r') as file:
        lines = file.readlines()
    
    with open(output_file, 'w') as out_file:
        for line in lines:
            line = line.strip()
            if not line:  # Skip empty lines
                continue
            starting_number, values_array = process_line(line)
            for index, value in enumerate(values_array, start=starting_number):
                out_file.write(f"setbyte({index}, 0x{value});\n")

# Adjust the file names as necessary
input_file_name = "romcap1.ptp"
output_file_name = "processed_values.txt"

process_file(input_file_name, output_file_name)