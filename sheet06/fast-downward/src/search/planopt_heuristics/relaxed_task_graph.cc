#include "relaxed_task_graph.h"

#include <iostream>
#include <vector>

using namespace std;

namespace planopt_heuristics {
RelaxedTaskGraph::RelaxedTaskGraph(const TaskProxy &task_proxy)
    : relaxed_task(task_proxy),
    variable_node_ids(relaxed_task.propositions.size()) {
    for (const Proposition &proposition : relaxed_task.propositions) {
        variable_node_ids[proposition.id] = graph.add_node(NodeType::OR);
    }

    initial_node_id = graph.add_node(NodeType::AND);
    for (PropositionID id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[id], initial_node_id);
    }

    for (const RelaxedOperator &op : relaxed_task.operators) {
        NodeID precondition_node_id = graph.add_node(NodeType::AND);
        for (PropositionID pre : op.preconditions) {
            graph.add_edge(precondition_node_id, variable_node_ids[pre]);
        }

        // We set the cost of an effect node here
        NodeID effect_node_id = graph.add_node(NodeType::AND, op.cost);
        for (PropositionID eff : op.effects) {
            graph.add_edge(variable_node_ids[eff], effect_node_id);
        }

        graph.add_edge(effect_node_id, precondition_node_id);
    }

    goal_node_id = graph.add_node(NodeType::AND);
    for (PropositionID goal : relaxed_task.goal) {
        graph.add_edge(goal_node_id, variable_node_ids[goal]);
    }
}

void RelaxedTaskGraph::change_initial_state(const State &state) {
    /* Remove all initial edges that where introduced for
       relaxed_task.initial_state. */
    for (PropositionID id : relaxed_task.initial_state) {
        graph.remove_edge(variable_node_ids[id], initial_node_id);
    }

    // Switch initial state of relaxed_task
    relaxed_task.initial_state = relaxed_task.translate_state(state);

    // Add all initial edges for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[id], initial_node_id);
    }
}

int RelaxedTaskGraph::cost_of_goal(CostAggregateType aggregate_type) {
    /* Compute the weighted most conservative valuation of the graph and
       use it to return the heuristic value of the goal node. */
    graph.weighted_most_conservative_valuation(aggregate_type);
    return graph.get_node(goal_node_id).aggregated_cost;
}

}
