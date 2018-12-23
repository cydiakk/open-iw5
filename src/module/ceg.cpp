#include <std_include.hpp>
#include "loader/module_loader.hpp"

class ceg final : public module
{
public:
	ceg()
	{
		OutputDebugStringA("+ CEG\n");
	}

	~ceg()
	{
		OutputDebugStringA("- CEG\n");
	}
};

REGISTER_MODULE(ceg)