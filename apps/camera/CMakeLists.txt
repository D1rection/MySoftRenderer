set(CAMERA_TRY_SOURCES
  simple.cpp
  better.cpp
)

foreach(src IN LISTS CAMERA_TRY_SOURCES)
  get_filename_component(basename "${src}" NAME_WE)
  add_executable("camera_${basename}" "${src}")
  target_link_libraries("camera_${basename}"
    PRIVATE
      tiny_renderer
      $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
  )
endforeach()
