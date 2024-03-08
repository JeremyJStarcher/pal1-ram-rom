max_size = 16 * 1024;

def binary_file_to_c_array(input_file, output_file, array_name="binaryData", bytes_per_line=12):
    try:
        with open(input_file, "rb") as bin_file:
            data = bin_file.read()
            
            # Break the hex data into lines
            hex_data_lines = []
            for i in range(0, max_size, bytes_per_line):
                line = ", ".join("0x{:02X}".format(byte) for byte in data[i:i+bytes_per_line])
                hex_data_lines.append(f"    {line}")
            hex_data_formatted = ",\n".join(hex_data_lines)
            
            # Calculate the size of the array
            data_size = max_size

            # Format the array and size into strings
            array_str = f"unsigned char {array_name}[] = {{\n{hex_data_formatted}\n}};\n"
            size_str = f"unsigned int {array_name}Size = {data_size};\n"

            # Write the formatted data to the output C file
            with open(output_file, "w") as c_file:
                c_file.write(array_str)
                c_file.write(size_str)
                
        print(f"Conversion completed. Output file: {output_file}")
    except IOError as e:
        print(f"Error: {e}")

# Example usage:
binary_file_to_c_array("AT28C256.bin", "rom_ext.c", "rom_ext", 16)

