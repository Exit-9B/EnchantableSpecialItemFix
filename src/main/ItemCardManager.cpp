#include "ItemCardManager.h"

#ifndef SKYRIMVR
bool ItemCardManager::InstallHooks()
{
	auto& trampoline = SKSE::GetTrampoline();

	static REL::Relocation<std::uintptr_t> hook1{ Offset::ItemCard::ShowItemData, 0x1366 };
	static REL::Relocation<std::uintptr_t> hook2{ Offset::ItemCard::ShowItemData, 0x1377 };

	constexpr auto assembly = REL::make_pattern<"66 83 7F 20 00 75 18">();

	if (!assembly.match(hook1.address())) {
		logger::warn("Failed to install item card hook"sv);
		return false;
	}

	REL::safe_fill(hook1.address(), REL::NOP, 0x7);

	_GetEnchantmentDescription = trampoline.write_call<5>(
		hook2.address(),
		GetEnchantmentDescription);

	return true;
}

#else

bool ItemCardManager::InstallHooks()
{
	auto& trampoline = SKSE::GetTrampoline();

	static auto funcAddr = Offset::ItemCard::ShowItemData.address();
	static REL::Relocation<std::uintptr_t> hook1{ funcAddr + 0x10CA };
	static REL::Relocation<std::uintptr_t> hook2{ funcAddr + 0x10DA };

	constexpr auto assembly = REL::make_pattern<"66 83 7F 20 00 75 0E">();

	if (!assembly.match(hook1.address())) {
		logger::warn("Failed to install item card hook"sv);
		return false;
	}

	REL::safe_fill(hook1.address(), REL::NOP, 0x7);

	_GetEnchantmentDescription = trampoline.write_call<5>(
		hook2.address(),
		GetEnchantmentDescription);

	return true;
}
#endif

auto ItemCardManager::GetSingleton() -> ItemCardManager*
{
	static ItemCardManager singleton{};
	return std::addressof(singleton);
}

void ItemCardManager::GetEnchantmentDescription(
	const RE::ItemCard* a_itemCard,
	const RE::MagicItem* a_enchantment,
	RE::BSString& a_description)
{
	if (a_description.empty()) {
		_GetEnchantmentDescription(a_itemCard, a_enchantment, a_description);
	}
	else if (a_enchantment->IsDynamicForm()) {
		RE::BSString enchantmentDescription;
		_GetEnchantmentDescription(a_itemCard, a_enchantment, enchantmentDescription);

		a_description = fmt::format("{} {}"sv, a_description, enchantmentDescription);
	}
}
