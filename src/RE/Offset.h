#pragma once

#ifndef SKYRIMVR
namespace Offset
{
	namespace ItemCard
	{
		constexpr REL::ID ShowItemData(51897);
	}
}

#else

namespace Offset
{
	namespace ItemCard
	{
		constexpr REL::Offset ShowItemData(0x8BEBD0);
	}
}
#endif
