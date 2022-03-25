#!/usr/bin/env python3

import os
import json

from src import classes


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


def convert_spawn_info(spawn_info: dict, poke_id: int, name: str, male_chance: float) -> list[dict]:
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
            "male_chance": male_chance,
            "spawn_area": 1337,  # TODO: select correct ID of SpawnAreaType
            "probability": round(entry["rarity"] / MAX_RARITY, RARITY_ROUNDING),
            "conditions": [
                # TODO: add conditions
            ]
        })

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
