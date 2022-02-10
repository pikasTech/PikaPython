#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"

/* Used only to study and test the linker script.
 * This has example for constant data and initialized data.
 */

struct struct_example {
    int a;
    int b;
};

struct struct_example struct_var;
int var1;
int var2 = 1;
const int var3 = 2;

int main(void)
{
    var1 = var2;
    var1 = var3;
    struct_var.a = 1;
    struct_var.b = 2;
}

