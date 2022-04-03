Json::Value load_config(std::string filename) {
    Json::Value config;
    std::ifstream config_stream(filename, std::ifstream::binary);
    if (!config_stream.is_open()) {
        std::cerr << "Configuration file " << filename << " not found. Exiting." << std::endl;
        exit(1);
    }
    config_stream >> config;
    return config;
}


std::string generate_new_uuid4() {
    return "00000000-0000-0000-0000-000000000000";  // TODO: actually generate version 4 UUIDs here
}


Json::Value make_point(const osmium::geom::Coordinates &coordinates) {
    if (coordinates.valid()) {
        Json::Value point = Json::Value(Json::arrayValue);
        point.append(coordinates.x);
        point.append(coordinates.y);
        return point;
    }
    std::cerr << "Failed to validate the coordinate at " << coordinates.x << "/" << coordinates.y << std::endl;
    return Json::nullValue;
}


Json::Value make_point(const osmium::Location location) {
    if (location.valid()) {
        Json::Value point = Json::Value(Json::arrayValue);
        point.append(location.lon());
        point.append(location.lat());
        return point;
    }
    std::cerr << "Failed to validate the location at " << location.lon() << "/" << location.lat() << std::endl;
    return Json::nullValue;
}


osmium::Box get_bbox(std::string spec) {
    std::stringstream spec_stream(spec);
    std::string segment;
    std::vector<double> bounding_box_values;

    while (std::getline(spec_stream, segment, BBOX_SPLIT_CHAR)) {
        bounding_box_values.push_back(std::stod(segment));
    }
    if (bounding_box_values.size() != 4) {
        std::cerr << "The bounding box has to contain exactly four values for minX, minY, maxX and maxY." << std::endl;
        exit(2);
    }

    osmium::Box bbox(
            bounding_box_values.at(0),
            bounding_box_values.at(1),
            bounding_box_values.at(2),
            bounding_box_values.at(3)
    );
    return bbox;
}
