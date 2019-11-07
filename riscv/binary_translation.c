#include <stdio.h>

/*    	.text    
    	.globl main  
	main:   
    		movl $0x0, %ebx   
   		movl $0x0, %ecx    
    		jmp  here  
	tloop:  
    		mov  %ecx, %eax   
    		add  %eax, %ebx   
    		addl $0x1, %ecx  
	here:  
    		cmpl $0x63, %ecx    
    		jle  tloop   
	ret 
*/
int main(){				// Register Mappings
    int a = 0;          		// int a => [rbp - 0x8] => %ebx
    int i = 0;          		// int i => [rbp - 0x4] => %ecx
    for(i;i<=0x63;i++){
        a += i;
    }
    return 0;
} 
