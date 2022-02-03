#include "ItemCardManager.h"

bool ItemCardManager::InstallHooks()
{
	auto& trampoline = SKSE::GetTrampoline();

	static REL::Relocation<std::uintptr_t> hook1{ Offset::ItemCard::ShowItemData, 0x1366 };
	static REL::Relocation<std::uintptr_t> hook2{ Offset::ItemCard::ShowItemData, 0x1377 };

	constexpr auto assembly = REL::make_pattern<"66 83 7F 20 00 75 18">();

	if (!assembly.match(hook1.address())) {
		return false;
	}

	REL::safe_fill(hook1.address(), REL::NOP, 0x7);
	_GetEnchantmentDescription = trampoline.write_call<5>(
		hook2.address(),
		GetEnchantmentDescription);

	return true;
}

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
