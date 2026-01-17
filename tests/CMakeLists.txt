file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

foreach(src IN LISTS TEST_SOURCES)
  get_filename_component(test_name "${src}" NAME_WE)
  add_executable("${test_name}" "${src}")

  target_link_libraries("${test_name}"
    PRIVATE
      tiny_renderer
      $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
  )

  add_test(NAME "${test_name}" COMMAND "${test_name}")
endforeach()
