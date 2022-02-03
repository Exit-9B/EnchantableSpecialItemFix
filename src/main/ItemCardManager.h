#pragma once

#include "RE/Offset.h"

class ItemCardManager
{
public:
	~ItemCardManager() = default;
	ItemCardManager(const ItemCardManager&) = delete;
	ItemCardManager(ItemCardManager&&) = delete;
	ItemCardManager& operator=(const ItemCardManager&) = delete;
	ItemCardManager& operator=(ItemCardManager&&) = delete;

	static auto GetSingleton() -> ItemCardManager*;

	static bool InstallHooks();

private:
	ItemCardManager() = default;

	static void GetEnchantmentDescription(
		const RE::ItemCard* a_itemCard,
		const RE::MagicItem* a_enchantment,
		RE::BSString& a_description);

	inline static REL::Relocation<decltype(GetEnchantmentDescription)> _GetEnchantmentDescription;
};
