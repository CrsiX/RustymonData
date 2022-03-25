#!/usr/bin/env python3

import os
import json

SINGLE_STATS_DIR = "."
SINGLE_STATS_FILES = sorted(list(os.walk(SINGLE_STATS_DIR))[0][2])

ALL_POKEMON_DIR = "."
ALL_POKEMON_FILES = list(os.walk(ALL_POKEMON_DIR))[0][2]

def convert_all():
    complete_data = {"spawns": {}}
    pokemon_without_spawn = []

    min_rarity = 10000
    max_rarity = 0

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

        spawn_info = spawn_data["spawnInfos"]
        entry = {
            "name": name,
            "catch_rate_modifier": data["catchRate"],
            "male_chance": data["malePercent"] / 100,
            "min_level": data["spawnLevel"],
            "max_level": data["spawnLevel"] + data["spawnLevelRange"],
            "spawn_group": ""
        }
        complete_data["spawns"][poke_id] = entry

    complete_data["no_spawns"] = pokemon_without_spawn
    complete_data["min_rarity"] = min_rarity
    complete_data["max_rarity"] = max_rarity

    with open("conversion_result.json", "w") as fd:
        json.dump(complete_data, fd, indent=2)

    print("min", min_rarity, "max", max_rarity)


if __name__ == "__main__":
    convert_all()
