import time
import uuid as _uuid
from typing import List, Tuple, Union

from geojson import GeoJSON
from geojson.geometry import Point, LineString, Polygon

from . import __version__


class _GeoJSONBase(GeoJSON):
    @classmethod
    def to_instance(cls, ob, default=None, strict=False):
        raise NotImplemented


class GeoJSONPointProperties:
    pass


class GeoJSONPathProperties:
    pass


class GeoJSONAreaProperties:
    pass


class GeoJSONPoint(_GeoJSONBase):
    type: str = "Feature"
    id: Union[None, str, int] = None
    geometry: Point
    properties: GeoJSONPointProperties

    def errors(self):
        return self.geometry.errors()


class GeoJSONPath(_GeoJSONBase):
    type: str = "Feature"
    id: Union[None, str, int] = None
    geometry: LineString
    properties: GeoJSONPathProperties

    def errors(self):
        return self.geometry.errors()


class GeoJSONArea(_GeoJSONBase):
    type: str = "Feature"
    id: Union[None, str, int] = None
    geometry: Polygon
    properties: GeoJSONAreaProperties

    def errors(self):
        return self.geometry.errors()


class GeoJSONMap(_GeoJSONBase):
    type: str = "FeatureCollection"
    uuid: _uuid.UUID
    bbox: Tuple[float, float, float, float]
    timestamp: int
    creator_software: str
    features: List[Union[GeoJSONPoint, GeoJSONPath, GeoJSONArea]]

    def __init__(
            self,
            bbox: Tuple[float, float, float, float],
            features: List[Union[GeoJSONPoint, GeoJSONPath, GeoJSONArea]],
            uuid: Union[None, _uuid.UUID, str] = None
    ):
        super().__init__()
        self.bbox = bbox
        self.features = features

        self.uuid = (uuid if isinstance(uuid, _uuid.UUID) else _uuid.UUID(uuid)) or _uuid.uuid4()
        self.timestamp = round(time.time())
        self.creator_software = f"{__package__} {__version__}"

    def errors(self):
        return [e.errors() for e in self.features]
