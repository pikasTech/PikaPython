#include "test_mimiCore.h"
extern "C"
{
	#include "dataMemory.h"
}

extern DMEM_STATE DMEMS;
extern int error_num;
extern int memory_warnning_num;

int main()
{
	test_mimiCore(0);
}
