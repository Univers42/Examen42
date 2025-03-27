
/*
Assignment name  : reverse_bits
Expected files   : reverse_bits.c
Allowed functions:
--------------------------------------------------------------------------------

Write a function that takes a byte, reverses it, bit by bit (like the
example) and returns the result.

Your function must be declared as follows:

unsigned char	reverse_bits(unsigned char octet);

Example:

  1 byte
_____________
 0100  0001
     ||
     \/
 1000  0010
*/

unsigned char reverse_bits(unsigned char octet) {
    return  ((octet & 0b00000001) << 7) |
            ((octet & 0b00000010) << 5) |
            ((octet & 0b00000100) << 3) |
            ((octet & 0b00001000) << 1) |
            ((octet & 0b00010000) >> 1) |
            ((octet & 0b00100000) >> 3) |
            ((octet & 0b01000000) >> 5) |
            ((octet & 0b10000000) >> 7);
}
