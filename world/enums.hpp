#ifndef WORLD_GENERATOR_ENUMS_HPP
#define WORLD_GENERATOR_ENUMS_HPP

namespace rustymon {

    enum class SpawnType {
        ANCIENT,
        ARCTIC,
        BEACH,
        CAVE,
        CEMETERY,
        COMMERCIAL,
        CONSTRUCTION_SITE,
        CORAL_REEF,
        DESERT,
        DOCK,
        FARMLAND,
        FOREST,
        GARBAGE,
        GLACIER,
        GRASS,
        HEATH,
        HILLS,
        INDUSTRIAL,
        LAKE,
        LIBRARY,
        MEDICAL,
        MILITARY,
        MEADOW,
        MOUNTAIN,
        MOUNTAIN_TOP,
        NATURE_RESERVE,
        OCEAN,
        PARK,
        PLAYA,
        PLAYGROUND,
        POWER,
        QUARRY,
        RESIDENTIAL,
        RIVER,
        SAVANNA,
        SPORTS,
        URBAN,
        VULCAN,
        WETLAND,
        WOOD
    };

    enum class POIType {
        NONE,
        POKE_CENTER,
        PROFESSOR,
        ARCHAEOLOGIST,
        ATTACK_TUTOR,
        BREEDING,
        SAFARI,
        DOJO,
        SMITH,
        STADIUM,
        THEATRE,

        SHOP,
        SHOP_BALLS,
        SHOP_BUILDING,
        SHOP_BOOSTS,
        SHOP_FOOD,
        SHOP_FURNITURE,
        SHOP_HEALS,
        SHOP_LETTERS,
        SHOP_TM,
        SHOP_TOOLS,

        ARENA_NORMAL,
        ARENA_FIRE,
        ARENA_WATER,
        ARENA_GRASS,
        ARENA_ELECTRIC,
        ARENA_ICE,
        ARENA_FIGHTING,
        ARENA_POISON,
        ARENA_GROUND,
        ARENA_FLYING,
        ARENA_PSYCHIC,
        ARENA_BUG,
        ARENA_ROCK,
        ARENA_GHOST,
        ARENA_DARK,
        ARENA_DRAGON,
        ARENA_STEEL,
        ARENA_FAIRY
    };

    enum class StreetType {
        HIGHWAY,
        STREET,
        PATH,
        RAILS,
        WATER
    };

    enum class AreaType {
        UNDEFINED,
        SAND,
        STONE,
        WATER,
        ICE,
        FOREST,
        FARMLAND,
        MEADOW,
        URBAN,
        VULCAN,
        MILITARY
    };

    enum class MoonType {
        FULL,
        DECREASING,
        NEW,
        INCREASING,
        LUNAR_ECLIPSE,
        SUN_ECLIPSE,
        BLOODY
    };

    enum class TimeType {
        MORNING,
        NOON,
        EVENING,
        NIGHT
    };

    enum class WeatherType {
        CLEAR,
        PARTY_CLOUDY,
        CLOUDY,
        WINDY,
        FOGGY,
        RAINY,
        SNOWY,
        SANDSTORM,
        THUNDERSTORM,
        VOLCANIC_ERUPTION,
        EXTREME_WARNING
    };

    enum class TemperatureType {
        FREEZING,
        COLD,
        NORMAL,
        WARM,
        HOT
    };

}

#endif //WORLD_GENERATOR_ENUMS_HPP
