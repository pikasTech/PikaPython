#include "random.h"
#include <stdlib.h>
#include <time.h>

void random___init__(PikaObj *self){
    srand(pika_platform_getTick());
}

int random_randint(PikaObj *self, int a, int b){
    return rand() % (b - a + 1) + a;
}

pika_float random_random(PikaObj *self){
    return rand() / (pika_float)RAND_MAX;
}

int random_randrange(PikaObj *self, int start, int stop, int step){
    if (step == 0) {
        return random_randint(self, start, stop);
    }
    int n = (stop - start + step) / step;
    int r = random_randint(self, 0, n - 1);
    return start + r * step;
}

PikaObj* random_seed(PikaObj *self, int a){
    srand(a);
    return NULL;
}

pika_float random_uniform(PikaObj *self, pika_float a, pika_float b){
    return (b - a) * random_random(self) + a;
}
