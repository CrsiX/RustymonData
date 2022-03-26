#!/usr/bin/env python3

import os
import enum
import json
from typing import Optional, Type

from src import classes


PRINT_UNKNOWN_ITEM = False
PRINT_UNKNOWN_SPEC = False
PRINT_UNKNOWN_TAGS = False
PRINT_UNKNOWN_MULTIPLIER = False

MAX_RARITY = 300.0
RARITY_ROUNDING = 5

SINGLE_STATS_DIR = "."
SINGLE_STATS_FILES = sorted(list(os.walk(SINGLE_STATS_DIR))[0][2])

ALL_POKEMON_DIR = "."
ALL_POKEMON_FILES = list(os.walk(ALL_POKEMON_DIR))[0][2]

SPAWN_AREA_MAPPING_FILE = "."

ALL_SPAWN_INFO_KEYS = {
    "minLevel", "maxLevel", "tags", "spec", "rarityMultipliers", "typeID",
    "stringLocationTypes", "condition", "anticondition", "heldItems", "rarity"
}
ALL_CONDITION_KEYS = {
    "baseBlocks", "dimensions", "maxLightLevel", "stringBiomes", "temperature",
    "minY", "maxY", "neededNearbyBlocks", "weathers", "times"
}

TIMES_MAPPING = {
    "DAWN": [classes.TimeType.NIGHT, classes.TimeType.MORNING],
    "MORNING": [classes.TimeType.MORNING],
    "DAY": [classes.TimeType.MORNING, classes.TimeType.NOON],
    "MIDDAY": [classes.TimeType.NOON],
    "AFTERNOON": [classes.TimeType.NOON, classes.TimeType.EVENING],
    "DUSK": [classes.TimeType.EVENING],
    "NIGHT": [classes.TimeType.EVENING, classes.TimeType.NIGHT],
    "MIDNIGHT": [classes.TimeType.NIGHT]
}

MOON_MAPPING = {
    0: [classes.MoonType.FULL],
    1: [classes.MoonType.FULL, classes.MoonType.DECREASING],
    2: [classes.MoonType.DECREASING],
    3: [classes.MoonType.DECREASING, classes.MoonType.NEW],
    4: [classes.MoonType.NEW],
    5: [classes.MoonType.NEW, classes.MoonType.INCREASING],
    6: [classes.MoonType.INCREASING],
    7: [classes.MoonType.INCREASING, classes.MoonType.FULL],
}

BIOME_MAPPING = {
    "the_void": [],
    "plains": [],
    "sunflower_plains": [],
    "snowy_plains": [],
    "ice_spikes": [],
    "desert": [],
    "swamp": [],
    "forest": [],
    "flower_forest": [],
    "birch_forest": [],
    "dark_forest": [],
    "old_growth_birch_forest": [],
    "old_growth_pine_taiga": [],
    "old_growth_spruce_taiga": [],
    "taiga": [],
    "snowy_taiga": [],
    "savanna": [],
    "savanna_plateau": [],
    "windswept_hills": [],
    "windswept_gravelly_hills": [],
    "windswept_forest": [],
    "windswept_savanna": [],
    "jungle": [],
    "sparse_jungle": [],
    "bamboo_jungle": [],
    "badlands": [],
    "eroded_badlands": [],
    "wooded_badlands": [],
    "meadow": [],
    "grove": [],
    "snowy_slopes": [],
    "frozen_peaks": [],
    "jagged_peaks": [],
    "stony_peaks": [],
    "river": [],
    "frozen_river": [],
    "beach": [],
    "snowy_beach": [],
    "stony_shore": [],
    "warm_ocean": [],
    "lukewarm_ocean": [],
    "deep_lukewarm_ocean": [],
    "ocean": [],
    "deep_ocean": [],
    "cold_ocean": [],
    "deep_cold_ocean": [],
    "frozen_ocean": [],
    "deep_frozen_ocean": [],
    "mushroom_fields": [],
    "dripstone_caves": [],
    "lush_caves": [],
    "nether_wastes": [],
    "warped_forest": [],
    "crimson_forest": [],
    "soul_sand_valley": [],
    "basalt_deltas": [],
    "the_end": [],
    "end_highlands": [],
    "end_midlands": [],
    "small_end_islands": [],
    "end_barrens": []
}

ITEM_MAPPING = {
    "pixelmon:smoke_ball": 75,
    "pixelmon:razor_claw": 105,
    "pixelmon:razor_fang": 110,
    "pixelmon:charcoal": 136,
    "pixelmon:silver_powder": 146,
    "pixelmon:poison_barb": 142,
    "pixelmon:pecha_berry": 391,
    "pixelmon:sharp_beak": 144,
    "pixelmon:light_ball": 186,
    "pixelmon:grip_claw": 89,
    "pixelmon:moon_stone": 16,
    "pixelmon:snowball": 542,
    "pixelmon:absorb_bulb": 98,
    "pixelmon:big_mushroom": 39,
    "pixelmon:tiny_mushroom": 38,
    "pixelmon:shed_shell": 74,
    "pixelmon:soft_sand": 143,
    "pixelmon:quick_claw": 112,
    "pixelmon:kings_rock": 109,
    "pixelmon:twisted_spoon": 145,
    "pixelmon:focus_band": 113,
    "pixelmon:ever_stone": 202,
    "pixelmon:cell_battery": 99,
    "pixelmon:lagging_tail": 111,
    "pixelmon:metal_coat": 151,
    "pixelmon:black_sludge": 92,
    "pixelmon:big_pearl": 42,
    "pixelmon:pearl": 41,
    "pixelmon:psychic_seed": 538,
    "pixelmon:thick_club": 190,
    "pixelmon:protector": 206,
    "pixelmon:lucky_punch": 187,
    "pixelmon:dragon_scale": 203,
    "pixelmon:mystic_water": 137,
    "pixelmon:star_piece": 45,
    "pixelmon:stardust": 44,
    "pixelmon:electirizer": 207,
    "pixelmon:magmarizer": 208,
    "pixelmon:leftovers": 93,
    "pixelmon:deep_sea_scale": 194,
    "pixelmon:wide_lens": 107,
    "pixelmon:berry_juice": 234,
    "pixelmon:luminous_moss": 541,
    "pixelmon:moomoo_milk": 240,
    "pixelmon:lucky_egg": 76,
    "pixelmon:revive": 232,
    "pixelmon:potion": 217,
    "pixelmon:max_revive": 233,
    "pixelmon:bright_powder": 67,
    "pixelmon:mental_herb": 95,
    "pixelmon:power_herb": 97,
    "pixelmon:black_belt": 141,
    "pixelmon:magnet": 138,
    "pixelmon:iron_ball": 118,
    "pixelmon:hard_stone": 147,
    "pixelmon:sitrus_berry": 398,
    "pixelmon:oran_berry": 395,
    "pixelmon:deep_sea_tooth": 193,
    "pixelmon:sticky_barb": 117,
    "pixelmon:sun_stone": 17,
    "pixelmon:light_clay": 88,
    "pixelmon:prism_scale": 210,
    "pixelmon:icy_rock": 87,
    "pixelmon:damp_rock": 85,
    "pixelmon:heat_rock": 84,
    "pixelmon:spell_tag": 148,
    "pixelmon:kasib_berry": 436,
    "pixelmon:heart_scale": 49,
    "pixelmon:dragon_fang": 149,
    "pixelmon:metronome": 102,
    "pixelmon:miracle_seed": 139,
    "pixelmon:never_melt_ice": 140,
    "pixelmon:reaper_cloth": 209,
    "pixelmon:black_glasses": 150,
    "pixelmon:silk_scarf": 152,
    "pixelmon:sachet": 614,
    "pixelmon:whipped_dream": 613,
    "pixelmon:rawst_berry": 392,
    "pixelmon:aspear_berry": 393,
    "pixelmon:cheri_berry": 389,
    "pixelmon:grassy_seed": 540,
    "pixelmon:misty_seed": 539,
    "pixelmon:electric_seed": 537,
    "pixelmon:chesto_berry": 390,
    "pixelmon:persim_berry": 396,
    "pixelmon:leek": 191
}


def _to_enum(c, t: Type[enum.Enum]) -> Optional[enum.Enum]:
    try:
        return t(int(c))
    except ValueError:
        try:
            return t[c]
        except KeyError:
            pass


def convert_spawn_info(spawn_info: dict, poke_id: int, name: str, male_chance: float) -> list[dict]:
    def _get_any_condition() -> dict:
        return {
            "index": 1,
            "modifier": 1.0,
            "weathers": [int(x) + 1 for x in range(len(classes.WeatherType))],
            "moons": [int(x) + 1 for x in range(len(classes.MoonType))],
            "times": [int(x) + 1 for x in range(len(classes.TimeType))]
        }

    def map_item(item_id: str) -> Optional[int]:
        if item_id in ITEM_MAPPING:
            return ITEM_MAPPING[item_id]
        if PRINT_UNKNOWN_ITEM:
            print("Unknown item ID", item_id, "for", poke_id)
        return None

    result = []
    for entry in spawn_info:
        for k in entry:
            if k not in ALL_SPAWN_INFO_KEYS:
                print("Unknown key", k, "encountered for", poke_id)
        for k in list(entry.get("condition", {}).keys()) + list(entry.get("anticondition", {}).keys()):
            if k not in ALL_CONDITION_KEYS:
                print("Unknown condition key", k, "encountered for", poke_id)
        if entry.get("typeID", "pokemon") != "pokemon":
            print("Unknown typeID for", poke_id)

        result.append({
            "min_level": entry["minLevel"],
            "max_level": entry["maxLevel"],
            "held_items": [
                {"item": map_item(item["itemID"]), "chance": item["percentChance"] / 100}
                for item in entry.get("heldItems", [])
                if item["percentChance"] > 0 and map_item(item["itemID"]) is not None
            ],
            "male_chance": male_chance,
            "spawn_area": 1337,  # TODO: select correct ID of SpawnAreaType
            "chance": round(entry["rarity"] / MAX_RARITY, RARITY_ROUNDING),
            "conditions": [
                # TODO: add conditions
            ] or _get_any_condition()
        })

        if PRINT_UNKNOWN_SPEC and "spec" in entry and entry["spec"] != {"name": name}:
            print("Unknown spec for", name)
        if PRINT_UNKNOWN_MULTIPLIER and "rarityMultipliers" in entry:
            print("rarityMultipliers for", poke_id)
        if PRINT_UNKNOWN_TAGS and "tags" in entry:
            print("tags for", poke_id)

    return result


def convert_all():
    complete_data = {"spawns": {}}
    pokemon_without_spawn = []

    for filename in SINGLE_STATS_FILES:
        poke_id = int(filename.split(".")[0])
        if poke_id == 0:
            continue

        with open(os.path.join(SINGLE_STATS_DIR, filename)) as fd:
            data = json.load(fd)

        name = data["pokemon"]
        set_filename = name.title() + ".set.json"
        if set_filename not in ALL_POKEMON_FILES:
            pokemon_without_spawn.append({"id": poke_id, "name": name})
            continue

        spawn_details_file = os.path.join(ALL_POKEMON_DIR, set_filename)
        if not os.path.exists(spawn_details_file):
            continue

        with open(spawn_details_file) as fd:
            spawn_data = json.load(fd)
        complete_data["spawns"][poke_id] = convert_spawn_info(
            spawn_data["spawnInfos"], poke_id, name, data["malePercent"] / 100
        )

    complete_data["no_spawns"] = pokemon_without_spawn

    with open("conversion_result.json", "w") as fd:
        json.dump(complete_data, fd, indent=2)


if __name__ == "__main__":
    convert_all()
