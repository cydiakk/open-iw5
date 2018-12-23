#include <std_include.hpp>

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.zdata")
#pragma comment(linker, "/merge:.text=.zdata")
#pragma comment(linker, "/section:.zdata,re")
#pragma comment(linker, "/base:0x400000")

__declspec(thread) char tls_data[0x2000];

#pragma bss_seg(".cdummy")
char dummy_seg[BINARY_PAYLOAD_SIZE];

char stub_seg[0x100000];

#pragma data_seg(".zdata")
char zdata[200] = { 1 };
