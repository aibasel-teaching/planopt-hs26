#ifndef PLANOPT_HEURISTICS_PLANOPT_HEURISITC_H
#define PLANOPT_HEURISTICS_PLANOPT_HEURISITC_H

#include "and_or_graph.h"
#include "relaxed_task_graph.h"

#include "../heuristic.h"

namespace planopt_heuristics {
class PlanoptHeuristic : public Heuristic {
    CostAggregateType aggregate_type;
    double ip_factor;
    RelaxedTaskGraph relaxed_task_graph;
protected:
    virtual int compute_heuristic(const State &state) override;
public:
    PlanoptHeuristic(
        const std::shared_ptr<AbstractTask> &transform, bool cache_estimates,
        const std::string &description, utils::Verbosity verbosity,
        CostAggregateType aggregate_type, double ip_factor);
};
}
#endif
