#include "_random.h"
#include <stdlib.h>
#include <time.h>

void _random___init__(PikaObj* self) {
    srand(pika_platform_get_tick());
}

int _random_randint(PikaObj* self, int a, int b) {
    return rand() % (b - a + 1) + a;
}

pika_float _random_random(PikaObj* self) {
    return rand() / (pika_float)RAND_MAX;
}

int _random_randrange(PikaObj* self, int start, int stop, int step) {
    if (step == 0) {
        return _random_randint(self, start, stop);
    }
    int n = (stop - start + step) / step;
    int r = _random_randint(self, 0, n - 1);
    return start + r * step;
}

PikaObj* _random_seed(PikaObj* self, int a) {
    srand(a);
    return NULL;
}

pika_float _random_uniform(PikaObj* self, pika_float a, pika_float b) {
    return (b - a) * _random_random(self) + a;
}
