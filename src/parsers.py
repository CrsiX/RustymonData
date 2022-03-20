from typing import Optional

import osmread

from .classes import GeoJSONMap, GeoJSONPoint, GeoJSONPath, GeoJSONArea, Point, LineString, Polygon


def parse_node(node: osmread.Node) -> Optional[GeoJSONPoint]:
    pass


def parse_way(way: osmread.Way) -> Optional[GeoJSONPath]:
    pass


def parse_relation(relation: osmread.Relation) -> Optional[GeoJSONArea]:
    pass


def parse_file(filename: str, world: GeoJSONMap):
    iterator = osmread.parse_file(filename)
    nodes = []
    ways = []
    relations = []
    points = []
    paths = []
    areas = []

    for i in iterator:
        if isinstance(i, osmread.Relation):
            relations.append(i)
            v = parse_relation(i)
            if v is not None:
                areas.append(v)
        elif isinstance(i, osmread.Way):
            ways.append(i)
            v = parse_way(i)
            if v is not None:
                paths.append(v)
        elif isinstance(i, osmread.Node):
            nodes.append(i)
            v = parse_node(i)
            if v is not None:
                points.append(v)
        else:
            raise ValueError(f"Unknown element type: {type(i).__name__}")
