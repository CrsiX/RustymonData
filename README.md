# Rustymon Data Repository

## World generator using OSM data

See the `world` directory.

### File format

```json5
{
    // Identifying the file via UUID
    "uuid": "e8801bf4-83ea-4576-9486-d078e24723a9",
    // Bounding box used while creating the file
    "bbox": [1.4, -7.3, 1.6, -7.1],
    // File creation timestamp
    "timestamp": 1647391089,
    // File format version
    "version": 1,
    // List of streets in the snippet (only used for better map displays)
    "streets": [
        {
            // Category (OSM highway classification) of the street
            "type": 4,
            // List of GPS points of the way
            "points": [
                [1.2, 2.3],
                [1.7, 3.9],
                [1.5, 4.5],
                [0.9, 5.1]
            ],
            // OpenStreetMap object ID of the source way
            "oid": 12345
        },
    ],
    // List of in-game points of interest
    "poi": [
        {
            // IDs of spawn categories in the environment of this POI
            "spawns": [1, 2, 7],
            // Type of POI (shop, arena, poke center, ...)
            "type": 2,
            // GPS location
            "point": [-4.5, 2.7],
            // OpenStreetMap object ID of the source node
            "oid": 12345
        }
    ],
    // List of in-game areas used for coloring the map and spawn areas
    "areas": [
        {
            // IDs of spawn categories inside this area
            "spawns": [1, 2],
            // Type of the area used for map coloring (e.g. forest, water, ...)
            "type": 1,
            "points": [
                [1.2, 2.6],
                [2.1, 2.8],
                [2.9, 3.5],
                [2.1, 2.9],
                // the end node equals the start node
                [1.2, 2.6]
            ],
            // OpenStreetMap object ID of the source relation or way
            "oid": 12345
        }
    ]
}
```

## Mob spawning

See the `spawning` directory.
