add_library(core_eval_strats)
add_library(msk::eval_strats ALIAS core_eval_strats)

target_link_libraries(core_eval_strats
  PRIVATE
    core_deps
)

target_sources(core_eval_strats PRIVATE ForwardEvaluationStrategy.cpp)
