#include "dataStack.h"
#include "dataQueue.h"

Stack* New_Stack() {
    Args* args = New_args(NULL);
    args_setInt(args, "top", 0);
    Stack* stack = args;
    return stack;
}
int32_t stack_deinit(Stack* stack) {
    Args* args = stack;
    args_deinit(args);
    return 0;
}
int32_t stack_pushStr(Stack* stack, char* str) {
    Args* args = stack;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    /* add top */
    char* topStr = fast_itoa(buff, top);
    args_setInt(args, "top", top + 1);
    return args_setStr(args, topStr, str);
}
char* stack_popStr(Stack* queue, char* outBuff) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top") - 1;
    char buff[11];
    /* sub top */
    args_setInt(args, "top", top);
    char* topStr = fast_itoa(buff, top);
    strcpy(outBuff, args_getStr(args, topStr));
    args_removeArg(args, topStr);
    return outBuff;
}