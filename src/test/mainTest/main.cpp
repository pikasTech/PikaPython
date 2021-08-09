#include "test_mimiCore.h"
extern "C"
{
	#include "dataMemory.h"
	#include <stdlib.h>
}

extern DMEM_STATE DMEMS;
extern int32_t error_num;
extern int32_t memory_warnning_num;

int main()
{
	test_mimiCore(0);
}
