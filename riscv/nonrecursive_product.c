#include <stdio.h>

int product(int A, int B){
        int C = A;
        for(B;B>0;B--){
                A = A + C;
        }
        return A;
}

int main(){
        int A = 75;
        int B = 10;
		int result;
		result = product(A,B);
        return result;
}
