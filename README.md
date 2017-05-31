# CRC for C++
## Description
A simple to use CRC class for C++, with predefined configurations for common CRC variants.

### Features
1. 7 predefined variants of CRC-8
2. 19 predefined variants of CRC-16
3. 3 predefined variants of CRC-24
4. 9 predefined variants of CRC-32
5. 3 predefined variants of CRC-64
6. New variants can be added using only a typedef
7. Set of efficent reflection functions

### Bugs
1. CRC-64 does not currently work
2. 12 CRC variants do not produce the correct checksums when tested, this is noted in a comment next to their typedefs

### Requirements
1. C++11

## Usage Examples
### Calculating a CRC-32 Checksum
```C++
// This is the data we want to create a checksum for
const char* const data = "123456789"; 
const size_t size = strlen(data);

// Now we can call the CRC function
asmith::crc_32::checksum_t checksum = asmith::crc_32::calculate(data, size);
```

### Defining a new CRC variant
```C++
typedef asmith::crc<
  uint32_t, // The type that will become crc_24::checksum_t
  0x864CFB, // The polynomial that is used to generate the CRC lookup table
  false,    // Determines if the input bytes are reflected
  true,     // Determines if the output checksum is not'd before return
  0xB704CE, // The initial value of the checksum
  0x000000, // The value to xor the checksum by before return
  24        // The length of the checksum in bits, this is automatically generated based on the checksum type by default
> crc_24;
```
