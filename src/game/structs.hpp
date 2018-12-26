#pragma once

namespace game
{
	namespace native
	{
		enum bdNATType : uint8_t
		{
			BD_NAT_UNKNOWN = 0x0,
			BD_NAT_OPEN = 0x1,
			BD_NAT_MODERATE = 0x2,
			BD_NAT_STRICT = 0x3,
		};
	}
}
