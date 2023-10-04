#!/usr/bin/python3

"""Generates a .h file with the data of a binary file."""
def bin_pack(file):
    with open(file, "rb") as f:
        data = f.read()
    data = ", ".join([str(x) for x in data])
    with open(file + ".h", "w") as f:
        f.write("static const unsigned char data[] = {" + data + "};\n")
        f.write("static const unsigned int data_len = " + str(len(data)) + ";\n")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: binpack.py <file>")
        sys.exit(1)
    bin_pack(sys.argv[1])