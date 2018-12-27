#include <std_include.hpp>

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.main")
#pragma comment(linker, "/merge:.text=.main")
#pragma comment(linker, "/section:.main,re")
#pragma comment(linker, "/base:0x400000")

// Moved to main.cpp to enforce early linking
//__declspec(thread) char tls_data[TLS_PAYLOAD_SIZE];

#pragma bss_seg(".payload")
char payload_data[BINARY_PAYLOAD_SIZE];

#pragma data_seg(".main")
char main_data[200] = {1};
