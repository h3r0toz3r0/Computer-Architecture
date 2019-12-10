int main(){				// Register Mappings
    int a = 0;          // int a => [rbp - 0x8] => %ebx
    int i = 0;          // int i => [rbp - 0x4] => %ecx
    for(i;i<=0x63;i++){
        a += i;
    }
    return 0;
}