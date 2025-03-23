#include "efuncs.h"
#include <math.h>
#include <stdio.h>

int divideUp(int numerator, int denominator) {
  return (numerator % denominator == 0) ? numerator / denominator
                                        : (numerator / denominator) + 1;
}

float maxf(float number, float maxValue){
    return (number>maxValue) ? maxValue : number;
}
