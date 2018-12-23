#include <std_include.hpp>
#include "loader/module_loader.hpp"

class dw final : public module
{
public:
	dw()
	{
		OutputDebugStringA("+ DW\n");
	}

	~dw()
	{
		OutputDebugStringA("- DW\n");
	}
};

REGISTER_MODULE(dw)