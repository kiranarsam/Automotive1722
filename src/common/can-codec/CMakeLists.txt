project(CanCodec_Project)

add_library(cancodec STATIC
  "src/can_codec.c"
  "src/can_encode_decode_inl.c"
  "src/can_encode_decode_inl.h"
)

target_include_directories(cancodec
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)