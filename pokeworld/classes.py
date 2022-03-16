import time
import uuid as _uuid
from typing import List, Tuple, Union

from . import __format_version__


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


class PointOfInterest(_Base):
    type: Union[int, POIType]
    point: Tuple[float, float]
    spawns: List[Union[int, SpawnType]]

    def __init__(self, type: Union[int, POIType], point: Tuple[float, float], spawns: List[Union[int, SpawnType]]):
        super().__init__(type=int(type), point=(float(p) for p in point), spawns=[int(s) for s in spawns])


class Street(_Base):
    type: Union[int, StreetType]
    points: List[Tuple[float, float]]

    def __init__(self, type: Union[int, StreetType], points: List[Tuple[float, float]]):
        super().__init__(type=int(type), points=([float(c) for c in p] for p in points))


class Area(_Base):
    type: Union[int, AreaType]
    points: List[Tuple[float, float]]
    spawns: List[Union[int, SpawnType]]

    def __init__(
            self,
            type: Union[int, AreaType],
            points: List[Tuple[float, float]],
        spawns: List[Union[int, SpawnType]]
    ):
        super().__init__(
            type=int(type),
            points=([float(c) for c in p] for p in points),
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
            bbox=bbox,
            timestamp=int(kwargs.get("timestamp", round(time.time()))),
            version=int(kwargs.get("version", __format_version__)),
            points=kwargs.get("points", []),
            streets=kwargs.get("streets", []),
            areas=kwargs.get("areas", [])
        )
