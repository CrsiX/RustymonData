#!/usr/bin/env python3

import os
import json
from typing import Optional

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

ALL_SPAWN_INFO_KEYS = {
    "minLevel", "maxLevel", "tags", "spec", "rarityMultipliers", "typeID",
    "stringLocationTypes", "condition", "anticondition", "heldItems", "rarity"
}
ALL_CONDITION_KEYS = {
    'baseBlocks', 'dimensions', 'maxLightLevel', 'stringBiomes', 'temperature',
    'minY', 'maxY', 'neededNearbyBlocks', 'weathers', 'times'
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

ITEM_MAPPING = {}  # TODO: fill with actual values


def convert_spawn_info(spawn_info: dict, poke_id: int, name: str, male_chance: float) -> list[dict]:
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
            "probability": round(entry["rarity"] / MAX_RARITY, RARITY_ROUNDING),
            "conditions": [
                # TODO: add conditions
            ]
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
