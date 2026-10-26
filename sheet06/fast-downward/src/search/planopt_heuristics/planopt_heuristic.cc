#include "planopt_heuristic.h"

#include "and_or_graph.h"

#include "../plugins/plugin.h"
#include "../utils/logging.h"

using namespace std;

namespace planopt_heuristics {
PlanoptHeuristic::PlanoptHeuristic(
    const std::shared_ptr<AbstractTask> &transform, bool cache_estimates,
    const std::string &description, utils::Verbosity verbosity,
    CostAggregateType aggregate_type, double ip_factor)
    : Heuristic(transform, cache_estimates, description, verbosity),
      aggregate_type(aggregate_type),
      ip_factor(ip_factor),
      relaxed_task_graph(task_proxy) {
    utils::g_log << "PlanoptHeuristic: Using aggregate type: "
                 << planopt_cost_aggregate_type_name(aggregate_type) << endl;
}

int PlanoptHeuristic::compute_heuristic(const State &state) {
    relaxed_task_graph.change_initial_state(state);
    if (aggregate_type != CostAggregateType::IP) {
        int h = relaxed_task_graph.cost_of_goal(aggregate_type);
        if (h >= numeric_limits<int>::max())
            return DEAD_END;
        return h;
    }

    assert(aggregate_type == CostAggregateType::IP);

    // Exercise 6.3b: Compute the heuristic value of the ip heuristic.
    // Use the double ip_factor and the function
    // relaxed_task_graph.cost_of_goal(...).
    utils::exit_with(utils::ExitCode::SEARCH_NOT_IMPLEMENTED);
    return DEAD_END;
}

static plugins::TypedEnumPlugin<CostAggregateType> _cost_aggregate_type_plugin({
    {"max", "maximum successor cost"},
    {"add", "sum successor costs"},
    {"ip", "ip successor costs"},
});

class PlanoptHeuristicFeature
    : public plugins::TypedFeature<TaskIndependentEvaluator> {
public:
    PlanoptHeuristicFeature() : TypedFeature("planopt_h") {
        document_title(
            "h heuristic from the lecture with different aggregation types");

        add_heuristic_options_to_feature(*this, "planopt_h");
        add_option<CostAggregateType>(
            "cost_aggregation",
            "Option to specify the cost aggregation type for the heuristic. "
            "Possible values are: max, add, ip.",
            "max");
        add_option<double>(
            "ip_factor",
            "Option to specify the  factor for the cost aggregation "
            "type. This option is only used if the cost aggregation type is set "
            "to 'ip'.",
            "0.5", plugins::Bounds("0.0", "1.0"));
    }

    virtual shared_ptr<TaskIndependentEvaluator> create_component(
        const plugins::Options &opts) const override {
        return components::make_auto_task_independent_component<
            PlanoptHeuristic, Evaluator>(
            get_heuristic_arguments_from_options(opts),
            opts.get<CostAggregateType>("cost_aggregation"),
            opts.get<double>("ip_factor"));
    }
};

static plugins::FeaturePlugin<PlanoptHeuristicFeature> _plugin;
}
