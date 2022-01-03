const char py[] =
    "i = 0\n"
    "while i < 10000:\n"
    "    i = i + 1\n"
    "\n";

char* get_py(void){
    return (char *)py;
}