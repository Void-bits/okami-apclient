#include <string>
#include <unordered_map>

#include "logger.h"
#include "okami/data/maptype.hpp"
#include "okami/data/structs.hpp"
#include "okami/gamestateregistry.h"
#include "okami/memorymap.hpp"

namespace okami
{
namespace
{
bool initialized = false;

okami::CharacterStats previousStats;
okami::CollectionData previousCollection;
okami::TrackerData previousTracker;
std::array<okami::MapState, okami::MapTypes::NUM_MAP_TYPES> previousMapData;
okami::BitField<86> previousGameStateFlags;

const std::unordered_map<unsigned int, std::string> emptyMapDesc{};

template <typename... Args> void warn(const char *format, Args... args)
{
    std::string newFmt = std::string("[Undocumented] ") + format;
    logWarning(newFmt.c_str(), args...);
}

template <unsigned N>
void compareBitfield(const char *name, okami::BitField<N> &old, okami::BitField<N> &current, const std::unordered_map<unsigned, std::string> &documentation,
                     bool showAlways = false)
{
    okami::BitField<N> diff = current ^ old;
    std::vector<unsigned> diffIndices = diff.GetSetIndices();
    for (auto idx : diffIndices)
    {
        if (!documentation.contains(idx))
        {
            warn("BitField %s index %u was changed from %d to %d", name, idx, old.IsSet(idx), current.IsSet(idx));
        }
        else if (showAlways)
        {
            logInfo("BitField %s index %u (%s) was changed from %d to %d", name, idx, documentation.at(idx).c_str(), old.IsSet(idx), current.IsSet(idx));
        }
    }
}

template <class T> void compareInt(const char *name, T old, T current);

template <> void compareInt<uint32_t>(const char *name, uint32_t old, uint32_t current)
{
    if (current != old)
    {
        warn("%s was changed from %08X to %08X", name, old, current);
    }
}

template <> void compareInt<uint16_t>(const char *name, uint16_t old, uint16_t current)
{
    if (current != old)
    {
        warn("%s was changed from %04X to %04X", name, old, current);
    }
}

template <> void compareInt<uint8_t>(const char *name, uint8_t old, uint8_t current)
{
    if (current != old)
    {
        warn("%s was changed from %02X to %02X", name, old, current);
    }
}

void comparePreviousStats()
{
    okami::CharacterStats &current = *okami::AmmyStats.get_ptr();
    okami::CharacterStats &old = previousStats;

    compareInt("CharacterStats::unk1", old.unk1, current.unk1);
    compareInt("CharacterStats::unk1b", old.unk1b, current.unk1b);
}

void comparePreviousCollection()
{
    const auto &registry = okami::GameStateRegistry::instance();
    okami::CollectionData &current = *okami::AmmyCollections.get_ptr();
    okami::CollectionData &old = previousCollection;

    compareInt("CollectionData::unk1", old.unk1, current.unk1);
    compareInt("CollectionData::unk2", old.unk2, current.unk2);
    compareInt("CollectionData::unk3", old.unk3, current.unk3);
    compareInt("CollectionData::unk4", old.unk4, current.unk4);

    compareInt("WorldStateData::unk1", old.world.unk1, current.world.unk1);
    compareInt("WorldStateData::unk2", old.world.unk2, current.world.unk2);
    compareInt("WorldStateData::unk3", old.world.unk3, current.world.unk3);
    compareInt("WorldStateData::unk4", old.world.unk4, current.world.unk4);

    auto &keyItemsFound = registry.getGlobalConfig().keyItemsFound;
    auto &goldDustsFound = registry.getGlobalConfig().goldDustsFound;
    compareBitfield("WorldStateData::keyItemsAcquired", old.world.keyItemsAcquired, current.world.keyItemsAcquired, keyItemsFound);
    compareBitfield("WorldStateData::goldDustsAcquired", old.world.goldDustsAcquired, current.world.goldDustsAcquired, goldDustsFound);

    compareInt("WorldStateData::unk10", old.world.unk10, current.world.unk10);
    for (unsigned i = 0; i < 56; i++)
    {
        std::string name = std::string("WorldStateData::unk11[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk11[i], current.world.unk11[i]);
    }

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
    {
        std::string name = std::string("WorldStateData::mapStateBits[") + std::to_string(i) + "] (" + okami::MapTypes::GetName(i) + ")";
        auto mapType = static_cast<MapTypes::Enum>(i);
        auto &worldStateBits = registry.getMapConfig(mapType).worldStateBits;
        compareBitfield(name.c_str(), old.world.mapStateBits[i], current.world.mapStateBits[i], worldStateBits, true);
    }

    auto &animalsFound = registry.getGlobalConfig().animalsFound;
    compareBitfield("WorldStateData::animalsFedBits", old.world.animalsFedBits, current.world.animalsFedBits, animalsFound);

    compareInt("WorldStateData::currentFortuneFlags", old.world.currentFortuneFlags, current.world.currentFortuneFlags);

    for (unsigned i = 0; i < 3; i++)
    {
        std::string name = std::string("WorldStateData::unk15[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk15[i], current.world.unk15[i]);
    }

    compareInt("WorldStateData::unk16", old.world.unk16, current.world.unk16);
    for (unsigned i = 0; i < 4; i++)
    {
        std::string name = std::string("WorldStateData::unk17[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk17[i], current.world.unk17[i]);
    }
    compareInt("WorldStateData::unk18", old.world.unk18, current.world.unk18);
    compareInt("WorldStateData::unk19", old.world.unk19, current.world.unk19);
    compareInt("WorldStateData::unk20", old.world.unk20, current.world.unk20);

    for (unsigned i = 0; i < 194; i++)
    {
        std::string name = std::string("WorldStateData::unk22[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk22[i], current.world.unk22[i]);
    }

    for (unsigned i = 0; i < 7; i++)
    {
        std::string name = std::string("WorldStateData::unk24[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk24[i], current.world.unk24[i]);
    }
}

void compareTrackerData()
{
    const auto &registry = okami::GameStateRegistry::instance();
    okami::TrackerData &current = *okami::AmmyTracker.get_ptr();
    okami::TrackerData &old = previousTracker;

    auto &gameProgress = registry.getGlobalConfig().gameProgress;
    auto &animalsFedFirstTime = registry.getGlobalConfig().animalsFedFirstTime;
    compareBitfield("TrackerData::gameProgressionBits", old.gameProgressionBits, current.gameProgressionBits, gameProgress, true);
    compareBitfield("TrackerData::animalsFedFirstTime", old.animalsFedFirstTime, current.animalsFedFirstTime, animalsFedFirstTime);

    compareBitfield("TrackerData::field_34", old.field_34, current.field_34, emptyMapDesc);
    compareBitfield("TrackerData::field_38", old.field_38, current.field_38, emptyMapDesc);

    auto &brushUpgrades = registry.getGlobalConfig().brushUpgrades;
    compareBitfield("TrackerData::brushUpgrades", old.brushUpgrades, current.brushUpgrades, brushUpgrades);

    compareBitfield("TrackerData::optionFlags", old.optionFlags, current.optionFlags, emptyMapDesc);

    auto &areasRestored = registry.getGlobalConfig().areasRestored;
    compareBitfield("TrackerData::areasRestored", old.areasRestored, current.areasRestored, areasRestored);

    compareInt("TrackerData::field_52", old.field_52, current.field_52);
    compareInt("TrackerData::unk2", old.unk2, current.unk2);
    compareInt("TrackerData::field_6D", old.field_6D, current.field_6D);
    compareInt("TrackerData::field_6E", old.field_6E, current.field_6E);
    compareInt("TrackerData::field_6F", old.field_6F, current.field_6F);
}

void comparePreviousMapData()
{
    const auto &registry = okami::GameStateRegistry::instance();
    auto &current = *okami::MapData.get_ptr();
    auto &old = previousMapData;
    std::string name;

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES; i++)
    {
        auto mapType = static_cast<MapTypes::Enum>(i);
        const auto &mapConfig = registry.getMapConfig(mapType);
        std::string mapNamePrefix = std::string("(") + okami::MapTypes::GetName(i) + ") ";
        for (unsigned j = 0; j < 32; j++)
        {
            if (mapConfig.userIndices.contains(j))
                continue;

            name = mapNamePrefix + std::string("MapState::user[") + std::to_string(j) + "]";
            compareInt(name.c_str(), old[i].user[j], current[i].user[j]);
        }

        name = mapNamePrefix + std::string("MapState::collectedObjects");
        compareBitfield(name.c_str(), old[i].collectedObjects, current[i].collectedObjects, mapConfig.collectedObjects);

        name = mapNamePrefix + std::string("MapState::commonStates");
        auto &commonStates = registry.getGlobalConfig().commonStates;
        compareBitfield(name.c_str(), old[i].commonStates, current[i].commonStates, commonStates);

        name = mapNamePrefix + std::string("MapState::areasRestored");
        compareBitfield(name.c_str(), old[i].areasRestored, current[i].areasRestored, mapConfig.areasRestored);

        name = mapNamePrefix + std::string("MapState::treesBloomed");
        compareBitfield(name.c_str(), old[i].treesBloomed, current[i].treesBloomed, mapConfig.treesBloomed);

        name = mapNamePrefix + std::string("MapState::cursedTreesBloomed");
        compareBitfield(name.c_str(), old[i].cursedTreesBloomed, current[i].cursedTreesBloomed, mapConfig.cursedTreesBloomed);

        name = mapNamePrefix + std::string("MapState::fightsCleared");
        compareBitfield(name.c_str(), old[i].fightsCleared, current[i].fightsCleared, mapConfig.fightsCleared);

        name = mapNamePrefix + std::string("MapState::npcHasMoreToSay");
        compareBitfield(name.c_str(), old[i].npcHasMoreToSay, current[i].npcHasMoreToSay, mapConfig.npcs, true);

        name = mapNamePrefix + std::string("MapState::npcUnknown");
        compareBitfield(name.c_str(), old[i].npcUnknown, current[i].npcUnknown, mapConfig.npcs, true);

        name = mapNamePrefix + std::string("MapState::mapsExplored");
        compareBitfield(name.c_str(), old[i].mapsExplored, current[i].mapsExplored, mapConfig.mapsExplored);

        name = mapNamePrefix + std::string("MapState::field_DC");
        compareBitfield(name.c_str(), old[i].field_DC, current[i].field_DC, mapConfig.field_DC);

        name = mapNamePrefix + std::string("MapState::field_E0");
        compareBitfield(name.c_str(), old[i].field_E0, current[i].field_E0, mapConfig.field_E0);
    }
}

void compareOtherStates()
{
    const auto &registry = okami::GameStateRegistry::instance();
    okami::BitField<86> &current = *okami::GlobalGameStateFlags.get_ptr();
    okami::BitField<86> &old = previousGameStateFlags;

    auto &doc = registry.getGlobalConfig().globalGameState;
    compareBitfield("GlobalGameState", old, current, doc, true);
}

void saveData()
{
    previousStats = *okami::AmmyStats.get_ptr();
    previousCollection = *okami::AmmyCollections.get_ptr();
    previousTracker = *okami::AmmyTracker.get_ptr();
    previousMapData = *okami::MapData.get_ptr();
    previousGameStateFlags = *okami::GlobalGameStateFlags.get_ptr();
}
} // namespace

void devDataFinder_OnGameTick()
{
    if (!initialized)
    {
        saveData();
        initialized = true;
        return;
    }
    comparePreviousStats();
    comparePreviousCollection();
    compareTrackerData();
    comparePreviousMapData();
    compareOtherStates();

    saveData();
}
} // namespace okami
