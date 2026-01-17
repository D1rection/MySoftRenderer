add_library(tiny_renderer
  tgaimage.cpp
  model.cpp
)

target_include_directories(tiny_renderer
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
