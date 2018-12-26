#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

class dw final : public module
{
public:
	dw()
	{
		// TODO Patch DW
	}

	void post_load() override
	{
		utils::hook(SELECT_VALUE(0x6F40A0, 0x6EE1C0, 0x611310), bd_logger_stub, HOOK_JUMP).install()->quick();
	}

private:
	static void bd_logger_stub(int /*type*/, const char* const /*channelName*/, const char*, const char* const /*file*/,
	                           const char* const function, const unsigned int /*line*/, const char* const msg, ...)
	{
		char buffer[2048];

		va_list ap;
		va_start(ap, msg);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);
		printf("%s: %s\n", function, buffer);

		va_end(ap);
	}
};

REGISTER_MODULE(dw)
