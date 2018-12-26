#include <std_include.hpp>

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.main")
#pragma comment(linker, "/merge:.text=.main")
#pragma comment(linker, "/section:.main,re")
#pragma comment(linker, "/base:0x400000")

__declspec(thread) char tls_data[0x2000];

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];

#pragma data_seg(".main")
char main_data[200] = {1};
