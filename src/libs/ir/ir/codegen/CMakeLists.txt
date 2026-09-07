add_library(mir_codegen)
add_library(mir::codegen ALIAS mir_codegen)

target_link_libraries(mir_codegen
  PRIVATE
  ir_deps
)

target_sources(mir_codegen PUBLIC TextToken.cpp WildcardToken.cpp ConstantProvider.cpp Module.cpp)
