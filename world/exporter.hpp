namespace rustymon {

    class Exporter {

        using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
        using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

        static void apply_read(WorldGenerator &data_handler, const std::string &in_file) {
            const osmium::io::File input_file{in_file};

            osmium::area::Assembler::config_type assembler_config;
            assembler_config.create_empty_areas = false;

            osmium::area::MultipolygonManager <osmium::area::Assembler> mp_manager{assembler_config};

            osmium::relations::read_relations(input_file, mp_manager);
            index_type index;

            location_handler_type location_handler{index};
            location_handler.ignore_errors();

            osmium::io::Reader reader{input_file, osmium::io::read_meta::no};

            osmium::apply(reader, location_handler, data_handler,
                          mp_manager.handler([&data_handler](const osmium::memory::Buffer &area_buffer) {
                              osmium::apply(area_buffer, data_handler);
                          }));

            reader.close();
        }

    public:

        static void generate_world_for_file(const std::string &in_file, const std::string& out_file, const osmium::Box bbox, const std::string &config_file) {
            WorldGenerator data_handler(bbox, config_file);
            apply_read(data_handler, in_file);

            static Json::StreamWriterBuilder builder;
            builder["commentStyle"] = "None";
            builder["indentation"] = "  ";
            builder["enableYAMLCompatibility"] = true;

            std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
            std::ofstream output_file_stream(out_file);
            writer->write(data_handler.get_json_data(), &output_file_stream);
            output_file_stream.close();
        }

        static void generate_world_for_http(const std::string &in_file, const std::string &push_url, const std::string &auth_info, const std::string &config_file) {
            WorldGenerator data_handler(config_file);
            apply_read(data_handler, in_file);

            // TODO: Handle uploading the resulting data
        }

    };

    void export_to_file(const std::string &in_file, const std::string& out_file, const osmium::Box bbox, const std::string &config_file) {
        return Exporter::generate_world_for_file(in_file, out_file, bbox, config_file);
    }

    void export_to_http(const std::string &in_file, const std::string &push_url, const std::string &auth_info, const std::string &config_file) {
        return Exporter::generate_world_for_http(in_file, push_url, auth_info, config_file);
    }

}
