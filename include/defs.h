#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define TITLE "Planet (WIP Title)"
#define TARGET_FPS 60

/// Collisions
// Objects collide with categories in their mask bits
#define PLANET_CATEGORY_BITS 0b0000'0000'0000'0010
#define ITEM_MASK_BITS PLANET_CATEGORY_BITS
#define SPAWNER_MASK_BITS PLANET_CATEGORY_BITS
#define SENSOR_MASK_BITS PLANET_CATEGORY_BITS

/// Messages
#define MESSAGE_SPAWN_ENEMY             0
#define MESSAGE_RECEIVE_TL              1
#define MESSAGE_RECEIVE_EXT             2
#define MESSAGE_RECEIVE_ANG             3
#define MESSAGE_TOGGLE_VISIBILITY       4
#define MESSAGE_RECEIVE_DIR             5
#define MESSAGE_RECEIVE_TILES           6
#define MESSAGE_RECEIVE_SL              7
#define MESSAGE_RECEIVE_HEALTH          8
#define MESSAGE_TOGGLE_SHOW_HEALTH_BAR  9
#define MESSAGE_ADD_TO_INVENTORY        10
#define MESSAGE_TOGGLE_SHOW_INVENTORY   11
#define MESSAGE_APPLY_FORCE             12
#define MESSAGE_RECEIVE_MOTION_STATE    13
#define MESSAGE_RECEIVE_VEL             14
#define MESSAGE_RESOLVE_COLLISION       15
#define MESSAGE_SHIFT_INVENTORY         16
#define MESSAGE_USE_ITEM                17
#define MESSAGE_RECEIVE_BODY            18
#define MESSAGE_RECEIVE_MASS            19
#define MESSAGE_DESTROY_ENTITY          20
#define MESSAGE_RECEIVE_CEN             21

/// Health
#define PLAYER_HEATLH_COLOR glm::vec3(0, 1, 0)
#define ENEMY_HEALTH_COLOR glm::vec3(1, 0, 0)
#define DEFAULT_HEALTH_COLOR glm::vec3(0, 0, 1)

#define HEALTH_DEPLETION_RATE 0.001f

#define PLAYER_HEALTH_CATEGORY 0b0000'0000'0000'0010
#define ENEMY_HEALTH_CATEGORY  0b0000'0000'0000'0100
#define PLAYER_HEALTH_MASK (~PLAYER_HEALTH_CATEGORY)
#define ENEMY_HEALTH_MASK (~ENEMY_HEALTH_CATEGORY)

/// Objects
#define OBJECT_TYPE    0b0000'0000'0000'0001
#define AGENT_TYPE     0b0000'0000'0000'0010
#define ITEM_TYPE      0b0000'0000'0000'0100
#define PLAYER_TYPE    0b0000'0000'0000'1000
#define ENEMY_TYPE     0b0000'0000'0001'0000
#define PLANET_TYPE    0b0000'0000'0010'0000
#define SPAWNER_TYPE   0b0000'0000'0100'0000

#define OBJECT_FACING_THRESHOLD 0.4f
#define OBJECT_RUNNING_THRESHOLD 1.f
#define OBJECT_FALLING_THRESHOLD -0.1f
#define OBJECT_JUMPING_THRESHOLD 0.1f
#define OBJECT_IDLE_THRESHOLD (TARGET_FPS * 10)

#define OBJECT_NUM_MOTION_STATES 5
#define NUM_MOTION_STATES 5

/// Tiles
#define TILE_SIZE 1.f
#define TILE_HALF_SIZE (TILE_SIZE/2.f)
#define TILE_DX glm::vec2(TILE_SIZE, 0)
#define TILE_DY glm::vec2(0, TILE_SIZE)
#define TILE_DIMS glm::vec2(TILE_SIZE)
#define TILE_EXTENTS (TILE_DIMS/2.f)
#define TILE_INACTIVE_ALPHA 0.35

/// Agents
#define AGENT_STATE_SHOW_HEALTH 0b0000'0000'0000'0001
#define AGENT_STATE_ATTACKING   0b0000'0000'0000'0010

#define AGENT_SLOW_ACCELERATION 5.f
#define AGENT_NORMAL_ACCELERATION 10.f
#define AGENT_FAST_ACCELERATION 15.f

/// Player
#define PLAYER_COLOR glm::vec4(1)
#define PLAYER_INIT_HEALTH 100.f

#define PLAYER_DIMS glm::vec2(0.8, 1.7)
#define PLAYER_AREA (PLAYER_DIMS.x*PLAYER_DIMS.y)
#define PLAYER_HALF_DIMS (PLAYER_DIMS/2.0f)
#define PLAYER_EXTENTS PLAYER_HALF_DIMS

#define PLAYER_INIT_POS glm::vec2(0, PLAYER_DIMS.y*1.1)

#define PLAYER_DENSITY 120.20569f
#define PLAYER_FRICTION 0.57721f
#define PLAYER_RESTITUTION 0
#define PLAYER_MASS (PLAYER_DENSITY*PLAYER_AREA)

#define PLAYER_FORCE (AGENT_NORMAL_ACCELERATION*PLAYER_MASS)
#define PLAYER_JUMP_FORCE (PLAYER_FORCE*20.f)
#define PLAYER_TILT_FORCE  (PLAYER_MASS*3.f)
#define PLAYER_MAX_SPEED glm::vec2(15.f, 30.f)

#define PLAYER_WEAK_ATT_ACCEL 25.f
#define PLAYER_WEAK_ATT_FORCE (12*ENEMY_UNIT_AREA*PLAYER_WEAK_ATT_ACCEL)

#define PLAYER_ANIMATION_FILE "resources/images/HeavyBandit_Spritesheet.png"
#define PLAYER_ANIMATION_DIMS glm::ivec2(7, 8)
#define PLAYER_MOTION_ANIMATIONS {{0,1,1}, {8,8,8}, {34,1,1}, {34,1,1}, {4,1,1}}

#define PLAYER_STATE_SHOW_INVENTORY 0b0000'0000'0001'0000

/// Enemies
#define ENEMY_MIN_INIT_HEALTH (PLAYER_INIT_HEALTH/20.f)
#define NORMAL_ENEMY_INIT_HEALTH (PLAYER_INIT_HEALTH/5.f)
#define ENEMY_MAX_INIT_HEALTH (PLAYER_INIT_HEALTH*10.f)
#define ENEMY_UNIT_DIMS glm::vec2(PLAYER_DIMS.x)
#define ENEMY_UNIT_EXTENTS (ENEMY_UNIT_DIMS/2.f)
#define ENEMY_MIN_EXTENTS (glm::min(ENEMY_UNIT_EXTENTS.x, ENEMY_UNIT_EXTENTS.y)/2.f)
#define ENEMY_MAX_EXTENTS (glm::max(ENEMY_UNIT_EXTENTS.x, ENEMY_UNIT_EXTENTS.y)*5.f)
#define ENEMY_UNIT_AREA (ENEMY_UNIT_DIMS.x*ENEMY_UNIT_DIMS.y)
#define NORMAL_ENEMY_MAX_SPEED (0.8f*PLAYER_MAX_SPEED)
#define ENEMY_MIN_MAX_SPEED (glm::min(PLAYER_MAX_SPEED.x, PLAYER_MAX_SPEED.y)/3.f)
#define ENEMY_MAX_MAX_SPEED (glm::max(PLAYER_MAX_SPEED.x, PLAYER_MAX_SPEED.y)*2.f)

/// Spawners
#define NORMAL_SPAWNER_INIT_HEALTH (10.f*NORMAL_ENEMY_INIT_HEALTH)
#define SMALL_SPAWNER_DIMS (1.5f*ENEMY_UNIT_DIMS)
#define SPAWNER_SLOWEST_RATE 30.f
#define SPAWNER_FASTEST_RATE 1.f
#define SPAWNER_SLOW_RATE 10.f
#define SPAWNER_MAX_SPEED ITEM_MAX_SPEED

#define SPAWNER_LONGEST_TIME (30.f*TARGET_FPS)
#define SPAWNER_SHORTEST_TIME (1.f*TARGET_FPS)
#define SPAWNER_LONG_TIME (10.f*TARGET_FPS)

/// Items
#define MIN_ITEM_SIZE glm::vec2(PLAYER_DIMS.y/(2.f*(float)M_PI))
#define SMALL_ITEM_SIZE (PLAYER_DIMS.y/(float)M_PI)
#define MAX_ITEM_SIZE glm::vec2(PLAYER_DIMS.y/2.f)
#define SMALL_ITEM_EXTENTS glm::vec2(SMALL_ITEM_SIZE/2.f)
#define ITEM_ICON_SIZE (9.f/M_PI)
#define ITEM_ICON_DIMS glm::vec2(ITEM_ICON_SIZE)
#define ITEM_MAX_SPEED glm::vec2(0, 20)

/// Planets
#define PLANET_MIN_GRAVITY (-PLANET_MAX_GRAVITY)
#define PLANET_MAX_GRAVITY (AGENT_FAST_ACCELERATION*1.5f)

/// Worlds
#define WORLD_DRAW_PLAYER_EXTRAS_FLAG  0b0000'0000'0001'0000
#define WORLD_DONT_DRAW_PLAYER_FLAG    0b0000'0000'0010'0000

/// GUI
#define WORLDEDITOR_GUI_DIMS glm::vec2(0.375f, 1.0f)
#define GUI_TEXT_MAX_LENGTH  50
#define GUI_PLANET_TAB 0
#define GUI_TILE_TAB 1
#define GUI_ITEM_TAB 2
#define GUI_ENEMY_TAB 3
#define GUI_SPAWNER_TAB 4
#define GUI_OBSTACLE_TAB 5

/// Animations
#define ANIMATION_MIN_SPEED 0.1f
#define ANIMATION_MAX_SPEED 20.f

/// Rendering
#define DEFAULT_CAMERA_CENTER glm::vec2(PLAYER_INIT_POS.x, PLAYER_INIT_POS.y)
#define DEFAULT_CAMERA_DIMENSIONS (PLAYER_DIMS * 8.f)

#define DEBUG_BOX2D_COLOR glm::vec4(0.5,0,0.3,1)
#define DEBUG_BOX2D_AABB_COLOR glm::vec4(1,0,0,1)

/// Text
#define MEDIUM_TEXT_HEIGHT 5

/// Timing
#define STANDARD_POPUP_TIME TARGET_FPS
#define HOLD_TIME (TARGET_FPS*0.75)

/// Screens
#define MAINMENU_SCREEN_INDEX 0
#define MAINGAME_SCREEN_INDEX 1
#define WORLDEDITOR_SCREEN_INDEX 2

#endif // DEFS_H_INCLUDED