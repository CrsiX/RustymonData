#!/usr/bin/env python3

import os
import enum
import json
from typing import Optional, Type

import common


PRINT_UNKNOWN_ITEM = False
PRINT_UNKNOWN_SPEC = False
PRINT_UNKNOWN_TAGS = False
PRINT_UNKNOWN_MULTIPLIER = False

MAX_RARITY = 300.0
RARITY_ROUNDING = 5

SKIPPED_POKEMON = list(range(650, 1024))

CONVERSION_RESULT_FILE = "spawn_probabilities.json"

SINGLE_STATS_DIR = "."
SINGLE_STATS_FILES = sorted(list(os.walk(SINGLE_STATS_DIR))[0][2])

ALL_POKEMON_DIR = "."
ALL_POKEMON_FILES = list(os.walk(ALL_POKEMON_DIR))[0][2]

ITEM_MAPPING_FILE = "item_mapping.json"
with open(ITEM_MAPPING_FILE) as _f:
    ITEM_MAPPING = json.load(_f)
del _f

ALL_SPAWN_INFO_KEYS = {
    "minLevel", "maxLevel", "tags", "spec", "rarityMultipliers", "typeID",
    "stringLocationTypes", "condition", "anticondition", "heldItems", "rarity"
}
ALL_CONDITION_KEYS = {
    "baseBlocks", "dimensions", "maxLightLevel", "stringBiomes", "temperature",
    "minY", "maxY", "neededNearbyBlocks", "weathers", "times"
}

TIMES_MAPPING = {
    "DAWN": [common.TimeType.NIGHT, common.TimeType.MORNING],
    "MORNING": [common.TimeType.MORNING],
    "DAY": [common.TimeType.MORNING, common.TimeType.NOON],
    "MIDDAY": [common.TimeType.NOON],
    "AFTERNOON": [common.TimeType.NOON, common.TimeType.EVENING],
    "DUSK": [common.TimeType.EVENING],
    "NIGHT": [common.TimeType.EVENING, common.TimeType.NIGHT],
    "MIDNIGHT": [common.TimeType.NIGHT]
}

MOON_MAPPING = {
    0: [common.MoonType.FULL],
    1: [common.MoonType.FULL, common.MoonType.DECREASING],
    2: [common.MoonType.DECREASING],
    3: [common.MoonType.DECREASING, common.MoonType.NEW],
    4: [common.MoonType.NEW],
    5: [common.MoonType.NEW, common.MoonType.INCREASING],
    6: [common.MoonType.INCREASING],
    7: [common.MoonType.INCREASING, common.MoonType.FULL],
}

WEATHER_MAPPING = {
    "CLEAR": [],
    "RAIN": [],
    "STORM": []
}


def convert_spawn_info(spawn_info: dict, poke_id: int, name: str, male_chance: float) -> list[dict]:
    def _get_conditions(cons, antis) -> list[dict]:
        cons = cons or {}
        antis = antis or {}

        times = set(int(x) + 1 for x in range(len(common.TimeType)))
        if "times" in cons:
            times = set()
            for t in cons["times"]:
                times |= set(TIMES_MAPPING[t])
        if "times" in antis:
            for t in antis["times"]:
                for e in TIMES_MAPPING[t]:
                    if e in times:
                        times.remove(e)

        temperatures = set(int(x) + 1 for x in range(len(common.TemperatureType)))
        if "temperatures" in cons:
            temperatures = set()
            for t in cons["temperatures"]:
                temperatures.add(common.to_enum(t, common.TemperatureType, strict=True))
        if "temperatures" in antis:
            for t in antis["temperatures"]:
                if t in temperatures:
                    temperatures.remove(common.to_enum(t, common.TemperatureType, strict=True))

        weathers = set(int(x) + 1 for x in range(len(common.WeatherType)))
        if "weathers" in cons:
            weathers = set()
            for w in cons["weathers"]:
                weathers |= set(WEATHER_MAPPING[w])
        if "weathers" in antis:
            for w in antis["weathers"]:
                for e in WEATHER_MAPPING[w]:
                    if e in weathers:
                        weathers.remove(e)

        return [{
            "index": 1337,  # TODO
            "modifier": 1.0,
            "weathers": sorted(weathers),
            "moons": [int(x) + 1 for x in range(len(common.MoonType))],
            "times": sorted(times),
            "temperatures": sorted(temperatures)
        }]

    def map_item(item_id: str) -> Optional[int]:
        if item_id in ITEM_MAPPING:
            return ITEM_MAPPING[item_id]
        if item_id.replace("pixelmon:", "") in ITEM_MAPPING:
            return ITEM_MAPPING[item_id.replace("pixelmon:", "")]
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
            "form": 0,  # TODO: add handling of different forms (e.g. seasons or mutations)
            "min_level": entry["minLevel"],
            "max_level": entry["maxLevel"],
            "held_items": [
                {"item": map_item(item["itemID"]), "probability": item["percentChance"] / 100}
                for item in entry.get("heldItems", [])
                if item["percentChance"] > 0 and map_item(item["itemID"]) is not None
            ],
            "female_probability": (1 - male_chance) if male_chance != -1 else -1,
            "spawn_areas": [1337],  # TODO: select correct ID of SpawnAreaType
            "conditions": _get_conditions(entry.get("condition"), entry.get("anticondition")),
            "probability": round(entry["rarity"] / MAX_RARITY, RARITY_ROUNDING),
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
    skipped_pokemon = []
    pokemon_without_spawn = []

    for filename in SINGLE_STATS_FILES:
        poke_id = int(filename.split(".")[0])
        if poke_id == 0:
            continue
        if poke_id in SKIPPED_POKEMON:
            skipped_pokemon.append(poke_id)
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
        male_percent = (data["malePercent"] / 100) if data["malePercent"] != -1 else -1
        complete_data["spawns"][poke_id] = convert_spawn_info(
            spawn_data["spawnInfos"], poke_id, name, male_percent
        )

    complete_data["no_spawns"] = pokemon_without_spawn
    complete_data["skipped"] = skipped_pokemon

    with open(CONVERSION_RESULT_FILE, "w") as fd:
        json.dump(complete_data, fd, indent=2)


if __name__ == "__main__":
    convert_all()
