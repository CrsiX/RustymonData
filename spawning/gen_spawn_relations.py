#!/usr/bin/env python3

"""
Script to interactively generate a JSON file mapping spawn areas with various probabilities to Pokemon species
"""

import os
import sys
import json

import common


def _to_int(s: str) -> int:
    try:
        return int(s)
    except ValueError:
        print(f"Invalid integer '{s}'!")


def get_spawn_area(p_id: int) -> int:
    spawn_area = None
    while spawn_area is None:
        spawn_area = common.to_enum(input(f"Selected spawn area for {p_id}: "), common.SpawnType, strict=True)
    return spawn_area


def get_probability(p_id: int) -> float:
    probability = None
    while probability is None:
        probability = input(f"Selected probability for {p_id}: ")
        try:
            return float(probability)
        except ValueError:
            pass
        probability = None


def get_conditions(p_id: int) -> list:
    if not input(f"Any conditions for {p_id}? (Enter for No, anything for Yes) "):
        return [common.get_any_condition()]
    conditions = []
    print("Start with the most general condition, then enter the most specific conditions!")

    more_conditions = True
    while more_conditions:
        weathers = [int(x) + 1 for x in range(len(common.WeatherType))]
        moons = [int(x) + 1 for x in range(len(common.MoonType))]
        times = [int(x) + 1 for x in range(len(common.TimeType))]

        w = input("Any weather conditions? (Enter = any weather okay, list of weathers = selected weather conditions) ")
        if w:
            weathers = [common.to_enum(x, common.WeatherType, strict=True) for x in w.split(" ")]
            print(f"Using {weathers!r}.")
            weathers = [int(x) for x in weathers]
        m = input("Any moon conditions? (Enter = any moon okay, list of moons = selected moon conditions) ")
        if m:
            moons = [common.to_enum(x, common.MoonType, strict=True) for x in m.split(" ")]
            print(f"Using {moons!r}.")
            moons = [int(x) for x in moons]
        t = input("Any time conditions? (Enter = any time okay, list of times = selected time conditions) ")
        if t:
            times = [common.to_enum(x, common.TimeType, strict=True) for x in t.split(" ")]
            print(f"Using {times!r}.")
            times = [int(x) for x in times]

        modifier = None
        while modifier is None:
            modifier = input("Modifier for this condition: ")
            try:
                modifier = float(modifier)
                if modifier < 0:
                    modifier = None
            except ValueError:
                modifier = None

        conditions.append(common.Condition(
            index=len(conditions) + 1,
            modifier=float(modifier),
            weathers=weathers,
            moons=moons,
            times=times
        ))
        more_conditions = input("More conditions? (Enter for No, anything for Yes) ")

    return conditions


def main():
    if len(sys.argv) < 2:
        print("Missing argument 'data file'.", file=sys.stderr)
        exit(2)
    data_file = sys.argv[1]
    data = {}
    if os.path.exists(data_file):
        with open(data_file, "r") as f:
            data = json.load(f)
    print(f"Loaded {len(data)} existing spawn relations.")
    print("Quit with Ctrl+C.")

    try:
        pokemon_id = _to_int(input("Pokemon ID: "))
        while pokemon_id is None or pokemon_id:
            if pokemon_id is not None:
                if pokemon_id not in data:
                    data[pokemon_id] = []
                spawn_area = get_spawn_area(pokemon_id)
                probability = get_probability(pokemon_id)
                conditions = get_conditions(pokemon_id)
                data[pokemon_id].append(common.SpawnRelation(
                    spawn_area=spawn_area, probability=probability, conditions=conditions
                ))
            pokemon_id = _to_int(input("Pokemon ID: "))
    finally:
        with open(data_file, "w") as f:
            json.dump(data, f, indent=2)


if __name__ == "__main__":
    main()
