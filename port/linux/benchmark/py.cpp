

const char py[] =
    "a = 0\n"
    "for i in range(0, 10000):\n"
    "    a = a + 1\n"
    "\n";

char* get_py(void){
    return (char *)py;
}