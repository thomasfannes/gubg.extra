#include "gubg/parse/polymorphic_tree/Parser.hpp"
#include "gubg/parse/polymorphic_tree/FixedStructure.hpp"

#include <iostream>
#include <list>
#include <string>

using namespace std;

struct Node
{
    std::string name;
    unsigned int count;
    std::list<std::string> include_dir;
    std::list<std::string> dependent_nodes;
};

struct Root
{
    std::list<Node> nodes;
};

using namespace gubg::parse::polymorphic_tree;

std::string test_data = "[root] { [node](name:a)(count:4) { [include_path](path:./) [include_path](path:src) [depends_on](uri:test) [include_path](path:inc) [depends_on](uri:testA) } [node](name:a)(count:4) { [include_path](path:./) [include_path](path:src) [depends_on](uri:test) [include_path](path:inc) [depends_on](uri:testA) } }";

int main()
{
    FixedStructureElement<std::list<Root> > r;
    FixedStructureElement<Root> & p = r.node<Root>("root", [](std::list<Root> & dst, const Root & src) { dst.push_back(src); });
    FixedStructureElement<Node> & n = p.composite_node<Node>("node", [](Root & r, const Node & d) {r.nodes.push_back(d); });

    n.attribute("name", [](Node & n, const std::string & v) {n.name = v; });
    n.attribute("count", [](Node & n, const std::string & v) {n.count = stoul(v); });

    {
        n.composite_node<std::string>("include_path", [](Node & n, const std::string & d) { n.include_dir.push_back(d); }).attribute("path", [](std::string & s, const std::string & d) { s = d; });
    }

    {
        n.composite_node<std::string>("depends_on", [](Node & n, const std::string & d) { n.dependent_nodes.push_back(d); }).attribute("uri", [](std::string & s, const std::string & d) { s = d; });
    }

    Parser pr;
    pr.set_root(&r);

    pr.process(test_data);

    return 0;
}
