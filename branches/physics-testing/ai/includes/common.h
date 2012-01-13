#ifndef COMMON_H
#define COMMON_H
enum game_types{
  agent_type  = 1 << 1,
  static_type = 1 << 2,
  camera_type = 1 << 3,
  wall_type   = 1 << 4,
  floor_type  = 1 << 5,
  debug_type  = 1 << 6
};
#endif
