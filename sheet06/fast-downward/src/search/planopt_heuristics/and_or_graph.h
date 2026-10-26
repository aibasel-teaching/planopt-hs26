#ifndef PLANOPT_HEURISTICS_AND_OR_GRAPH_H
#define PLANOPT_HEURISTICS_AND_OR_GRAPH_H

#include "../utils/hash.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace planopt_heuristics {

using NodeID = int;
enum class NodeType {
    AND,
    OR
};

enum class CostAggregateType {
    MAX,
    ADD,
    IP,
};

std::string planopt_cost_aggregate_type_name(CostAggregateType aggregate_type);

struct AndOrGraphNode {
    NodeID id;
    NodeType type;
    bool forced_true;
    int num_forced_successors;
    std::vector<NodeID> successor_ids;
    std::vector<NodeID> predecessor_ids;

    int direct_cost;
    int aggregated_cost;

    AndOrGraphNode(NodeID id, NodeType type, int direct_cost)
        : id(id), type(type), direct_cost(direct_cost) {
    }
};

class AndOrGraph {
    std::vector<int> successor_costs;
    std::vector<AndOrGraphNode> nodes;
    utils::HashMap<std::vector<NodeID>, NodeID> and_node_ids;
    utils::HashMap<std::vector<NodeID>, NodeID> or_node_ids;

public:
    NodeID add_node(NodeType type, int weight = 0);
    void add_edge(NodeID from, NodeID to);
    void remove_edge(NodeID from, NodeID to);

    const AndOrGraphNode &get_node(NodeID id) const;
    void weighted_most_conservative_valuation(CostAggregateType aggregate_type);
};

}
#endif
