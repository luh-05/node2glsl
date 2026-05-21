find_package(Doxygen REQUIRED)

set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/doxygen)

configure_file(
  ${CMAKE_SOURCE_DIR}/Doxyfile.in
  ${CMAKE_BINARY_DIR}/Doxyfile
  @ONLY
)

add_custom_target(doc_doxygen
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
  COMMAND ${CMAKE_COMMAND} ${CMAKE_BINARY_DIR/Doxyfile}
  COMMENT "Generating Doxygen docs"
)
