#include "and_or_graph.h"

#include "../utils/collections.h"
#include "../utils/system.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>

using namespace std;

namespace planopt_heuristics {

string planopt_cost_aggregate_type_name(CostAggregateType aggregate_type) {
    switch (aggregate_type) {
    case CostAggregateType::MAX:
        return "max";
    case CostAggregateType::ADD:
        return "add";
    case CostAggregateType::IP:
        return "ip";
    default:
        return "unknown";
    }
}

NodeID AndOrGraph::add_node(NodeType type, int weight) {
    NodeID id = nodes.size();
    nodes.emplace_back(id, type, weight);
    return id;
}

void AndOrGraph::add_edge(NodeID from, NodeID to) {
    nodes[from].successor_ids.push_back(to);
    nodes[to].predecessor_ids.push_back(from);
}

template<typename T>
void remove_from_vector(vector<T> &v, const T &elem) {
    v.erase(remove(v.begin(), v.end(), elem), v.end());
}

void AndOrGraph::remove_edge(NodeID from, NodeID to) {
    /* Note that this is an inefficient way of removing the edge. We would need
       a different data structure to make this more efficient. */
    remove_from_vector(nodes[from].successor_ids, to);
    remove_from_vector(nodes[to].predecessor_ids, from);
}

const AndOrGraphNode &AndOrGraph::get_node(NodeID id) const {
    assert(utils::in_bounds(id, nodes));
    return nodes[id];
}

void AndOrGraph::weighted_most_conservative_valuation(
    CostAggregateType aggregate_type) {
    /*
      General approach for computing the weighted most conservative valuation.
    */

    using QueueEntry = pair<int, NodeID>;
    /*
      Priority queues usually order entries so the largest entry is the first.
      By using the comparator greater<T> instead of the default less<T>, we
      change the ordering to sort the smallest element first.
    */
    priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> queue;

    for (AndOrGraphNode &node : nodes) {
        node.forced_true = false;
        node.num_forced_successors = 0;
        if (node.type == NodeType::AND && node.successor_ids.empty()) {
            node.aggregated_cost = 0;
            queue.push({0, node.id});
        } else {
            node.aggregated_cost = numeric_limits<int>::max();
        }
    }

    while (!queue.empty()) {
        QueueEntry cost_id_pair = queue.top();
        queue.pop();
        NodeID id = cost_id_pair.second;
        AndOrGraphNode &node = nodes[id];
        assert(
            (node.type == NodeType::AND &&
             node.num_forced_successors ==
                 static_cast<int>(node.successor_ids.size())) ||
            (node.type == NodeType::OR && node.num_forced_successors >= 1));
        if (node.forced_true) {
            // We handled this node already.
            continue;
        }
        node.forced_true = true;
        for (NodeID pre_id : node.predecessor_ids) {
            AndOrGraphNode &pre = nodes[pre_id];
            ++pre.num_forced_successors;
            int num_successors = pre.successor_ids.size();
            assert(pre.num_forced_successors >= 1);
            assert(pre.num_forced_successors <= num_successors);

            if (pre.type == NodeType::OR) {
                int new_cost = node.aggregated_cost + pre.direct_cost;
                assert(node.aggregated_cost < numeric_limits<int>::max());
                assert(new_cost < numeric_limits<int>::max());
                assert(new_cost >= 0);
                if (new_cost < pre.aggregated_cost) {
                    pre.aggregated_cost = new_cost;
                    queue.push({pre.aggregated_cost, pre.id});
                }
            } else if (
                pre.type == NodeType::AND &&
                pre.num_forced_successors == num_successors) {
                assert(pre.aggregated_cost == numeric_limits<int>::max());
                successor_costs.clear();
                for (NodeID succ_id : pre.successor_ids) {
                    const AndOrGraphNode &successor = nodes[succ_id];
                    assert(successor.forced_true);
                    assert(successor.aggregated_cost >= 0);
                    assert(
                        successor.aggregated_cost < numeric_limits<int>::max());
                    successor_costs.push_back(successor.aggregated_cost);
                }

                // Exercise 6.3a: Finalize the implementations for hmax and
                // hadd by aggregating the successor costs.
                // More precise: Aggregate successor_costs according to
                // aggregate_type and store the result in pre.aggregated_cost.
                if (aggregate_type == CostAggregateType::MAX) {
                    utils::exit_with(utils::ExitCode::SEARCH_NOT_IMPLEMENTED);
                } else if (aggregate_type == CostAggregateType::ADD) {
                    utils::exit_with(utils::ExitCode::SEARCH_NOT_IMPLEMENTED);
                } else {
                    cerr << "Unknown aggregate type: "
                         << planopt_cost_aggregate_type_name(aggregate_type)
                         << endl;
                    utils::exit_with(utils::ExitCode::SEARCH_NOT_IMPLEMENTED);
                }

                pre.aggregated_cost += pre.direct_cost;
                queue.push({pre.aggregated_cost, pre.id});
            }
        }
    }
}

}
