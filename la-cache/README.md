## Welcome to La Cache

La Cache is a simple column associative structure.

In addition to the cache, there are also materials for creating instruction traces/experimenting on work loads with dineroIV 

# Usage: ./cache <trace file>

# Description:
        8KB instruction cache
        32B block size
        32-bit address
        Column associative structure
        Reports miss rate     
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
        
![extra_credit_1_final](https://user-images.githubusercontent.com/31195314/68355000-ed761c80-00db-11ea-8796-529cd7d737cd.PNG)


