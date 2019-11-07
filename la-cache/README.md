# Usage: ./cache <trace file>
  
# Trace File Format:
  LABEL   = 0       read data               
          = 1       write data               
          = 2       instruction fetch 
          = 3       escape record (treated as unknown access type)
          = 4       escape record (causes cache flush)
   0 <= ADDRESS <= ffffffff where the hexadecimal addresses are NOT preceded by "0x." 

# Example Trace File Formats:
          2 0    This is an instruction fetch at hex address 0. 
          0 1000   This is a data read at hex address 1000. 
          1 70f60888  This is a data write at hex address 70f60888. 

# Description:

        8KB instruction cache
        32B block size
        32-bit address
        Column associative structure
        Reports miss rate     

