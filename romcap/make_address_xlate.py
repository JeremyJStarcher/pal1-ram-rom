array_length = 2**16
patch = [0 for _ in range(array_length)]

high = []
low = []
addr = []

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

def process_file(input_file, mode):
    global high
    global low
    global addr

    with open(input_file, 'r') as file:
        lines = file.readlines()

    cnt = -1

    for line in lines:
        cnt += 1
        line = line.strip()
        if not line:  # Skip empty lines
            continue
        starting_number, values_array = process_line(line)
        for index, value in enumerate(values_array, start=starting_number):

            number = int(value, 16)

            if mode == 1:
                addr.append(index)
                high.append(number)
                #print(high[index])
                #print(mode, index, value, number);
            if mode == 2:
                low.append( number)


def set_bit(value, bit, should_set):
    if should_set:
        return value | (1 << bit)
    else:
        return value & ~(1 << bit)

def check_bit(value, bit):
    return value & (1 << bit)

def bout(val):
    binary_str = f"{bin(val)[2:]:0>16}"  # Removes '0b' prefix and pads with zeros to 16 bits
    return binary_str

def swapbits(addr):
    mangled_addr = 0
    mangled_addr =set_bit(mangled_addr, 0, check_bit(addr, 0));
    mangled_addr =set_bit(mangled_addr, 1, check_bit(addr, 1));
    mangled_addr =set_bit(mangled_addr, 12, check_bit(addr, 2));
    mangled_addr =set_bit(mangled_addr, 2, check_bit(addr, 3));
    mangled_addr =set_bit(mangled_addr, 3, check_bit(addr, 4));
    mangled_addr =set_bit(mangled_addr, 4, check_bit(addr, 5));
    mangled_addr =set_bit(mangled_addr, 5, check_bit(addr, 6));
    mangled_addr =set_bit(mangled_addr, 6, check_bit(addr, 7));
    mangled_addr =set_bit(mangled_addr, 7, check_bit(addr, 8));
    mangled_addr =set_bit(mangled_addr, 8, check_bit(addr, 9));
    mangled_addr =set_bit(mangled_addr, 9, check_bit(addr, 10));
    mangled_addr =set_bit(mangled_addr, 10, check_bit(addr, 11));
    mangled_addr =set_bit(mangled_addr, 11, check_bit(addr, 12));
    mangled_addr =set_bit(mangled_addr, 13, check_bit(addr, 13));
    mangled_addr =set_bit(mangled_addr, 14, check_bit(addr, 14));
    mangled_addr =set_bit(mangled_addr, 15, check_bit(addr, 15));
    return mangled_addr


# Adjust the file names as necessary
output_file_name = "processed_values.txt"

process_file("high.ptp",  1)
process_file("low.ptp",  2)

xlate = []

for index, value in enumerate(addr):
    #print (index)
    #print(f"0x{value:X}") 
    #xlate.append(f"0x{high[index]:02X}{low[index]:02X}")
    xlate.append(high[index] * 256 + low[index])
#print(xlate)

has_dups = len(xlate) != len(set(xlate))

print("DOes it hae dups?",  has_dups, hex(len(xlate)), hex(len(set(xlate))))


for number in range(16-3):  # range(16) generates numbers from 0 to 15
    off = 1 << number
    ad = addr[off]
    fa = swapbits(ad)

    print(number, "------------------------")
    print(f"real: {bout(ad)}")
    print(f"xlat: {bout(xlate[off])}")
    print(f"fa:   {bout(fa)}")
    print(f"      {bout(off)}")


    print(f"ZZZZ: {bout(0xA004)}")
