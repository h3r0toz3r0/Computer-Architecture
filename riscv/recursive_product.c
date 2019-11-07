#include <stdio.h>

int product(int A, int B){
        if(B==0){
                return 0;
        }
        return A + product(A,--B);
}

int main(){
        int A = 3;
        int B = 18;

        printf("%d\n",product(A,B));
        return 0;
}
