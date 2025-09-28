/**
 * Build the result sequentially as it consumes the input.
 * 
 */
unsigned char reverse_bits(unsigned char octet)
{
    unsigned char reversed = 0;
    for (int i = 0; i < 8; i++)
    {
        reversed <<= 1;             // Shift the reversed byte to the left
        reversed |= (octet & 1);    // Get the least significant bit of octet and add it to reversed
        octet >>= 1;                // Shift the original byte to the right
    }
    return reversed;
}