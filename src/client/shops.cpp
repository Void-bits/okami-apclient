#include <cstdint>
#include <vector>

#include "okami/items.hpp"
#include "okami/maps.hpp"
#include "shop.h"

static ShopDefinition AgataForestShop;
static ShopDefinition ArkOfYamatoShop;
static ShopDefinition CityCheckpointShop;
static ShopDefinition DragonPalaceShop;
static ShopDefinition KamikiPostTeiShop;
static ShopDefinition KamikiShop;
static ShopDefinition KamikiPastShop;
static ShopDefinition KamuiShop;
static ShopDefinition KusaShop;
static ShopDefinition MoonCaveInteriorShop;
static ShopDefinition MoonCaveStaircaseShop;
static ShopDefinition NRyoshimaShop;
static ShopDefinition OniIslandInteriorShop;
static ShopDefinition PonctanShop;
static ShopDefinition RyoshimaShop;
static ShopDefinition SasaShop;
static ShopDefinition SeianWeaponShop;
static ShopDefinition SeianFishShop;
static ShopDefinition ShinshuShop;
static ShopDefinition TakaPassShop;
static ShopDefinition WawkuShrineShop;

static std::vector<okami::ItemShopStock> AgataFangShop;
static std::vector<okami::ItemShopStock> ArkOfYamatoFangShop;
static std::vector<okami::ItemShopStock> ImperialPalaceFangShop;

void InitializeShopData()
{
    // TODO update shops with AP information here

    KamikiShop.SetStock({
        {okami::ItemTypes::MarlinRod, 100},
        {okami::ItemTypes::ExorcismSlipM, 100},
    });

    // Special sell prices for some fish
    TakaPassShop.SetSellValues(okami::DefaultTakaPassItemSellPrices);
    SeianFishShop.SetSellValues(okami::DefaultSeianFishShopItemSellPrices);
}

const void *GetCurrentItemShopData(uint32_t shopNum)
{
    auto mapID = static_cast<okami::MapID::Enum>(okami::ExteriorMapID.get());

    switch (mapID)
    {
    case okami::MapID::AgataForestCursed:
    case okami::MapID::AgataForestHealed:
        return AgataForestShop.GetData();
    case okami::MapID::ArkofYamato:
        return ArkOfYamatoShop.GetData();
    case okami::MapID::CityCheckpoint:
        return CityCheckpointShop.GetData();
    case okami::MapID::DragonPalace:
        return DragonPalaceShop.GetData();
    case okami::MapID::KamikiVillagePostTei:
        return KamikiPostTeiShop.GetData();
    case okami::MapID::KamikiVillageCursed:
    case okami::MapID::KamikiVillage:
        return KamikiShop.GetData();
    case okami::MapID::KamikiVillagePast:
        return KamikiPastShop.GetData();
    case okami::MapID::KamuiCursed:
    case okami::MapID::KamuiHealed:
        return KamuiShop.GetData();
    case okami::MapID::KusaVillage:
        return KusaShop.GetData();
    case okami::MapID::MoonCaveInterior:
        return MoonCaveInteriorShop.GetData();
    case okami::MapID::MoonCaveStaircaseAndOrochiArena:
        return MoonCaveStaircaseShop.GetData();
    case okami::MapID::NRyoshimaCoast:
        return NRyoshimaShop.GetData();
    case okami::MapID::OniIslandLowerInterior:
        return OniIslandInteriorShop.GetData();
    case okami::MapID::Ponctan:
        return PonctanShop.GetData();
    case okami::MapID::RyoshimaCoastCursed:
    case okami::MapID::RyoshimaCoastHealed:
        return RyoshimaShop.GetData();
    case okami::MapID::SasaSanctuary:
        return SasaShop.GetData();
    case okami::MapID::SeianCityCommonersQuarter:
        switch (shopNum)
        {
        case 0:
            return SeianWeaponShop.GetData();
        case 1:
            return SeianFishShop.GetData();
        }
        break;
    case okami::MapID::ShinshuFieldCursed:
    case okami::MapID::ShinshuFieldHealed:
        return ShinshuShop.GetData();
    case okami::MapID::TakaPassCursed:
    case okami::MapID::TakaPassHealed:
        return TakaPassShop.GetData();
    case okami::MapID::WawkuShrine:
        return WawkuShrineShop.GetData();
    default:
        break;
    }
    return nullptr;
}

okami::ItemShopStock *GetCurrentDemonFangShopData(uint32_t *pNumItems)
{
    auto mapID = static_cast<okami::MapID::Enum>(okami::ExteriorMapID.get());

    switch (mapID)
    {
    case okami::MapID::AgataForestCursed:
    case okami::MapID::AgataForestHealed:
        *pNumItems = AgataFangShop.size();
        return AgataFangShop.data();
    case okami::MapID::ArkofYamato:
        *pNumItems = ArkOfYamatoFangShop.size();
        return ArkOfYamatoFangShop.data();
    case okami::MapID::ImperialPalaceAmmySize:
        *pNumItems = ImperialPalaceFangShop.size();
        return ImperialPalaceFangShop.data();
    default:
        break;
    }

    *pNumItems = 0;
    return nullptr;
}
