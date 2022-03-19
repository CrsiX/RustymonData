#include <iostream>

#include <osmium/handler.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/relations/relations_manager.hpp>
#include <osmium/visitor.hpp>

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


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: PokeWorldGenerator <InputFile>\n";
        return 2;
    }
    osmium::io::Reader reader{argv[1]};
    CounterHandler handler;
    osmium::apply(reader, handler);
    reader.close();
    std::cout << handler.nodes << " nodes "<< handler.ways << " ways " << handler.areas << " areas " << handler.relations << " relations\n";
}
