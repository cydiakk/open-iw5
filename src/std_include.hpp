#pragma once

#define BINARY_PAYLOAD_SIZE 0x0A000000

#pragma warning(push)
#pragma warning(disable: 4458)

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Windowsx.h>
#include <objidl.h>
#include <gdiplus.h>

#include <map>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>
#include <filesystem>

#pragma warning(pop)

using namespace std::literals;

#pragma comment (lib, "gdiplus.lib")

#pragma warning(disable: 4100)

#include "resource.hpp"
