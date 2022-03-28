import enum
import time
import uuid as _uuid
from typing import Any, List, Optional, Tuple, Type, Union


FORMAT_VERSION = 1


class _Base(dict):
    def __repr__(self) -> str:
        """
        Provide a fancy representation of the object
        """

        return f"{type(self).__name__}({', '.join(f'{str(k)}={repr(self[k])}' for k in self)})"

    def __getattr__(self, name):
        """
        Permit dictionary items to be retrieved like object attributes
        """

        try:
            return self[name]
        except KeyError:
            raise AttributeError(name)

    def __setattr__(self, name, value):
        """
        Permit dictionary items to be set like object attributes
        """

        self[name] = value

    def __delattr__(self, name):
        """
        Permit dictionary items to be deleted like object attributes
        """

        try:
            del self[name]
        except KeyError:
            raise AttributeError(name)


class SpawnType(enum.IntEnum):
    ANCIENT = enum.auto()
    ARCTIC = enum.auto()
    BEACH = enum.auto()
    CAVE = enum.auto()
    CEMETERY = enum.auto()
    COMMERCIAL = enum.auto()
    CONSTRUCTION_SITE = enum.auto()
    CORAL_REEF = enum.auto()
    DESERT = enum.auto()
    DOCK = enum.auto()
    FARMLAND = enum.auto()
    FOREST = enum.auto()
    GARBAGE = enum.auto()
    GLACIER = enum.auto()
    GRASS = enum.auto()
    HEATH = enum.auto()
    HILLS = enum.auto()
    INDUSTRIAL = enum.auto()
    LAKE = enum.auto()
    LIBRARY = enum.auto()
    MEDICAL = enum.auto()
    MILITARY = enum.auto()
    MEADOW = enum.auto()
    MOUNTAIN = enum.auto()
    MOUNTAIN_TOP = enum.auto()
    NATURE_RESERVE = enum.auto()
    OCEAN = enum.auto()
    PARK = enum.auto()
    PLAYA = enum.auto()
    PLAYGROUND = enum.auto()
    POWER = enum.auto()
    QUARRY = enum.auto()
    RESIDENTIAL = enum.auto()
    RIVER = enum.auto()
    SAVANNA = enum.auto()
    SPORTS = enum.auto()
    URBAN = enum.auto()
    VULCAN = enum.auto()
    WETLAND = enum.auto()
    WOOD = enum.auto()


class POIType(enum.IntEnum):
    NONE = enum.auto()
    POKE_CENTER = enum.auto()
    PROFESSOR = enum.auto()
    ARCHAEOLOGIST = enum.auto()
    ATTACK_TUTOR = enum.auto()
    BREEDING = enum.auto()
    SAFARI = enum.auto()
    DOJO = enum.auto()
    SMITH = enum.auto()
    STADIUM = enum.auto()
    THEATRE = enum.auto()

    SHOP = enum.auto()
    SHOP_BALLS = enum.auto()
    SHOP_BUILDING = enum.auto()
    SHOP_BOOSTS = enum.auto()
    SHOP_FOOD = enum.auto()
    SHOP_FURNITURE = enum.auto()
    SHOP_HEALS = enum.auto()
    SHOP_LETTERS = enum.auto()
    SHOP_TM = enum.auto()
    SHOP_TOOLS = enum.auto()

    ARENA_NORMAL = enum.auto()
    ARENA_FIRE = enum.auto()
    ARENA_WATER = enum.auto()
    ARENA_GRASS = enum.auto()
    ARENA_ELECTRIC = enum.auto()
    ARENA_ICE = enum.auto()
    ARENA_FIGHTING = enum.auto()
    ARENA_POISON = enum.auto()
    ARENA_GROUND = enum.auto()
    ARENA_FLYING = enum.auto()
    ARENA_PSYCHIC = enum.auto()
    ARENA_BUG = enum.auto()
    ARENA_ROCK = enum.auto()
    ARENA_GHOST = enum.auto()
    ARENA_DARK = enum.auto()
    ARENA_DRAGON = enum.auto()
    ARENA_STEEL = enum.auto()
    ARENA_FAIRY = enum.auto()


class StreetType(enum.IntEnum):
    HIGHWAY = enum.auto()
    STREET = enum.auto()
    PATH = enum.auto()
    RAILS = enum.auto()
    WATER = enum.auto()


class AreaType(enum.IntEnum):
    UNDEFINED = enum.auto()
    SAND = enum.auto()
    STONE = enum.auto()
    WATER = enum.auto()
    ICE = enum.auto()
    FOREST = enum.auto()
    FARMLAND = enum.auto()
    MEADOW = enum.auto()
    URBAN = enum.auto()
    VULCAN = enum.auto()
    MILITARY = enum.auto()


class MoonType(enum.IntEnum):
    FULL = enum.auto()
    DECREASING = enum.auto()
    NEW = enum.auto()
    INCREASING = enum.auto()
    LUNAR_ECLIPSE = enum.auto()
    SUN_ECLIPSE = enum.auto()
    BLOODY = enum.auto()


class TimeType(enum.IntEnum):
    MORNING = enum.auto()
    NOON = enum.auto()
    EVENING = enum.auto()
    NIGHT = enum.auto()


class WeatherType(enum.IntEnum):
    CLEAR = enum.auto()
    PARTY_CLOUDY = enum.auto()
    CLOUDY = enum.auto()
    WINDY = enum.auto()
    FOGGY = enum.auto()
    RAINY = enum.auto()
    SNOWY = enum.auto()
    SANDSTORM = enum.auto()
    THUNDERSTORM = enum.auto()
    VOLCANIC_ERUPTION = enum.auto()
    EXTREME_WARNING = enum.auto()


class TemperatureType(enum.IntEnum):
    FREEZING = enum.auto()
    COLD = enum.auto()
    NORMAL = enum.auto()
    WARM = enum.auto()
    HOT = enum.auto()


class PointOfInterest(_Base):
    type: Union[int, POIType]
    point: Tuple[float, float]
    spawns: List[Union[int, SpawnType]]

    def __init__(
            self,
            type: Union[int, POIType],  # noqa
            point: Tuple[float, float],
            spawns: List[Union[int, SpawnType]]
    ):
        super().__init__(
            type=int(type),
            point=tuple(float(p) for p in point),
            spawns=[int(s) for s in spawns]
        )


class Street(_Base):
    type: Union[int, StreetType]
    points: List[Tuple[float, float]]

    def __init__(
            self,
            type: Union[int, StreetType],  # noqa
            points: List[Tuple[float, float]]
    ):
        super().__init__(
            type=int(type),
            points=[tuple(float(c) for c in p) for p in points]
        )


class Area(_Base):
    type: Union[int, AreaType]
    points: List[Tuple[float, float]]
    spawns: List[Union[int, SpawnType]]

    def __init__(
            self,
            type: Union[int, AreaType],  # noqa
            points: List[Tuple[float, float]],
            spawns: List[Union[int, SpawnType]]
    ):
        super().__init__(
            type=int(type),
            points=[tuple(float(c) for c in p) for p in points],
            spawns=[int(s) for s in spawns]
        )


class World(_Base):
    uuid: str
    bbox: Tuple[float, float, float, float]
    timestamp: int
    version: int
    points: List[PointOfInterest]
    streets: List[Street]
    areas: List[Area]

    def __init__(self, bbox: Tuple[float, float, float, float], uuid: Union[None, str, _uuid.UUID] = None, **kwargs):
        super().__init__(
            uuid=(str(uuid) if isinstance(uuid, _uuid.UUID) else uuid and str(_uuid.UUID(uuid))) or str(_uuid.uuid4()),
            bbox=tuple(float(b) for b in bbox),
            timestamp=int(kwargs.get("timestamp", round(time.time()))),
            version=int(kwargs.get("version", FORMAT_VERSION)),
            points=kwargs.get("points", []),
            streets=kwargs.get("streets", []),
            areas=kwargs.get("areas", [])
        )


class Condition(_Base):
    index: int = 1
    modifier: float
    weathers: List[WeatherType]
    moons: List[MoonType]
    times: List[TimeType]
    temperatures: List[TemperatureType]


class SpawnRelation(_Base):
    spawn_area: SpawnType
    probability: float
    conditions: List[Condition]


def get_any_condition() -> Condition:
    return Condition(
        index=1,
        modifier=1.0,
        weathers=[to_enum(x, WeatherType) for x in range(len(WeatherType))],
        moons=[to_enum(x, MoonType) for x in range(len(MoonType))],
        times=[to_enum(x, TimeType) for x in range(len(TimeType))],
        temperatures=[to_enum(x, TemperatureType) for x in range(len(TemperatureType))]
    )


def to_enum(v: Any, t: Type[enum.IntEnum], strict: bool = False, echo: bool = False) -> Optional[enum.IntEnum]:
    try:
        v = int(v)
        if echo:
            print(f"Using {t(v)!r}.")
        return t(v)
    except ValueError:
        try:
            v = t[v]
            if echo:
                print(f"Using {v!r}.")
            return v
        except KeyError:
            pass
    if strict:
        raise ValueError(f"Unknown member {v} for type {t}")
