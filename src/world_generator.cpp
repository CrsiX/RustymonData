#include <iostream>
#include <jsoncpp/json/json.h>
#include <osmium/handler.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/relations/relations_manager.hpp>
#include <osmium/visitor.hpp>
#include <osmium/geom/factory.hpp>
#include "poke_world_enums.hpp"
#include "poke_world_fields.hpp"


class CounterHandler : public osmium::handler::Handler {
public:
    int nodes = 0;
    int ways = 0;
    int areas = 0;
    int relations = 0;

    void node(const osmium::Node& node) {
        this->nodes++;
    }

    void way(const osmium::Way& way) {
        this->ways++;
    }

    void area(const osmium::Area& area) {
        this->areas++;
    }

    void relation(const osmium::Relation& relation) {
        this->relations++;
    }
};




class MyManager : public osmium::relations::RelationsManager<MyManager, true, true, true> {
private:
    int node_count = 0;
    int way_count = 0;
    int relation_count = 0;
    Json::Value root;
    Json::Value nodes = Json::Value(Json::arrayValue);
    Json::Value ways = Json::Value(Json::arrayValue);
    Json::Value relations = Json::Value(Json::arrayValue);

    /// Determine whether a given relation should be ignored in the Poke World
    bool ignore_relation(const osmium::Relation& relation) {
        if (relation.tags().has_key("type")) {
            std::string type = (std::string) relation.tags().get_value_by_key("type");
            for (std::string s : IGNORED_RELATION_TYPES) {
                if (type == s) {
                    return true;
                }
            }
        } else {
            std::cerr << "Relation " << relation.id() << " has no 'type' tag." << std::endl;
        }
        return false;
    }

    /// Print an OSM object and its tags to stdout
    void print_object(const osmium::OSMObject& object) {
        std::cout << "Object " << object.id() << std::endl;
        for (auto &t : object.tags()) {
            std::cout << "  " << t.key() << "=" << t.value() << std::endl;
        }
    }

public:
    Json::Value get_root() {
        this->root["nodes"] = nodes;
        this->root["ways"] = ways;
        this->root["relations"] = relations;
        return this->root;
    }

    bool new_relation(const osmium::Relation& relation) {
        return true;
    }

    bool new_member(const osmium::Relation& relation, const osmium::RelationMember& member, std::size_t) {
        return true;
    }

};


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: PokeWorldGenerator <InputFile>\n";
        return 2;
    }
    /*
    osmium::io::Reader reader{argv[1]};
    CounterHandler handler;
    osmium::apply(reader, handler);
    reader.close();
    std::cout << handler.nodes << " nodes "<< handler.ways << " ways " << handler.areas << " areas " << handler.relations << " relations\n";
    */

    osmium::io::File input_file{argv[1]};
    MyManager manager;
    osmium::relations::read_relations(input_file, manager);
    osmium::io::Reader reader{input_file};
    osmium::apply(reader, manager.handler());
    osmium::memory::Buffer buffer = manager.read();

    Json::Value v = manager.get_root();
    v = manager.get_root();
    std::cout << v;
}
