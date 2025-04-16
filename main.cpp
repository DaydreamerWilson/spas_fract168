#include <stdint.h>
#include <stdlib.h>
#include <bitset>
#include <iostream>
#include <cstring>
#include <random>

#include "spas_fract168.hpp"

int main(){
    spas_fract168_t a(0b1000, 0x8888'0000'0000'0000, 0, 0);
    spas_fract168_t b(0b1000, 0x8000'0000'0000'0000, 0, 0);
    spas_fract168_t c(0b0000, 0x8000'0000'0000'0000, 0, 0);

    for(int i = 0; i < 5000; i++){
        a*=b;
        a.printAll();
        printf("\n");
        a*=c;
        a.printAll();
        printf("\n");
    }
}
