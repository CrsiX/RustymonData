```text
 ___              _                                  ___         _
|  _ \           ( )_                               (  _ \      ( )_
| (_) )_   _  ___|  _)_   _  ___ ___    _    ___    | | ) |  _ _|  _)  _ _
|    /( ) ( )  __) | ( ) ( )  _   _  \/ _ \/  _  \  | | | )/ _  ) |  / _  )
| |\ \| (_) |__  \ |_| (_) | ( ) ( ) | (_) ) ( ) |  | |_) | (_| | |_( (_| |
(_) (_)\___/(____/\__)\__  |_) (_) (_)\___/(_) (_)  (____/ \__ _)\__)\__ _)
                     ( )_| |
                      \___/
```

## World generator using OSM data

See the `world` directory.

### Output file format

```json5
{
    // Bounding box used while creating the file
    "bbox": [1.4, -7.3, 1.6, -7.1],
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

### Config file format

```json5
{
  // Definitions for worker threads
  "workers": {
    // Number of worker threads for node processing
    "node": 2,
    // Number of worker threads for way processing
    "way": 2,
    // Number of worker threads for area processing
    "area": 4,
    // Number of worker threads for uploading the final data via HTTP
    "upload": 4
  },
  // Definition of the size of a single resulting tile
  // (higher values lead to smaller map tiles)
  "size": {
    "x": 10000,
    "y": 10000
  },
  // Definition for Points Of Interest (POI)
  "poi": [
    {
      // POI type (integer of the 1-indexed enum representing different types of POI)
      "type": 2,
      // List of spawns at that POI (integer of the 1-indexed enum representing different spawn types), may be empty
      "spawns": [
        1,
        2,
        3
      ],
      // List of required OSM attributes, i.e. the node will be skipped if not present
      // (if the key is present and the value list is empty, the node is accepted; otherwise any value must be present)
      "required": {
        "key1": [
          "value1",
          "value2"
        ]
      },
      // List of forbidden OSM attributes, i.e. the node will be skipped if present
      // (if the key is present and the value list is empty, the node is rejected; otherwise any value must be present)
      "forbidden": {
        "key2": [
          "value3",
          "value4",
          "value5",
          "value6"
        ]
      }
    }
  ],
  // Definition for Streets (includes rivers and railways), see POI above for details (key 'spawns' is invalid here)
  "streets": [
    {
      "type": 3,
      "required": {
        "key1": [
          "value1",
          "value2"
        ]
      },
      "forbidden": {
        "key2": [
          "value3",
          "value4",
          "value5",
          "value6"
        ]
      }
    }
  ],
  // Definition for Areas, see POI above for details
  "areas": [
    {
      "type": 2,
      "spawns": [
        1,
        2,
        3
      ],
      "required": {
        "key1": [
          "value1",
          "value2"
        ]
      },
      "forbidden": {
        "key2": [
          "value3",
          "value4",
          "value5",
          "value6"
        ]
      }
    }
  ]
}
```

## Mob spawning

See the `spawning` directory.
