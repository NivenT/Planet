#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

/// Note: Many #define's here depend on other #include's appearing before them
///       in the files that include this header (e.g. glm). I'm not sure how I
///       feel about that

#define TITLE "Planet (WIP title)"
#define TARGET_FPS 60

/// Collisions
// Objects collide with categories in their mask bits
#define PLANET_CATEGORY_BITS 0b0000'0000'0000'0010
#define ITEM_MASK_BITS PLANET_CATEGORY_BITS

/// Objects
#define OBJECT_TYPE 0b0000'0000'0000'0001
#define AGENT_TYPE  0b0000'0000'0000'0010
#define ITEM_TYPE   0b0000'0000'0000'0100
#define PLAYER_TYPE 0b0000'0000'0000'1000
#define ENEMY_TYPE  0b0000'0000'0001'0000

/// Tiles
#define TILE_SIZE 10.f
#define TILE_DX glm::vec2(TILE_SIZE, 0)
#define TILE_DY glm::vec2(0, TILE_SIZE)
#define TILE_DIMS glm::vec2(TILE_SIZE)
#define TILE_EXTENTS (TILE_DIMS/2.f)

/// Player
#define PLAYER_COLOR glm::vec4(0.24, 0.00, 0.40, 1.00)
#define PLAYER_INIT_HEALTH 100.f
#define PLAYER_DIMS glm::vec2(TILE_SIZE/2.f, TILE_SIZE)
#define PLAYER_AREA (PLAYER_DIMS.x*PLAYER_DIMS.y)
#define PLAYER_HALF_DIMS (PLAYER_DIMS/2.0f)
#define PLAYER_EXTENTS PLAYER_HALF_DIMS
#define PLAYER_INIT_POS b2Vec2(0, PLAYER_DIMS.y*1.1)
#define PLAYER_DENSITY 1.2020569f
#define PLAYER_FRICTION 0.57721f
#define PLAYER_MASS (PLAYER_DENSITY*PLAYER_AREA)
#define PLAYER_FORCE (50.f*PLAYER_MASS)
#define PLAYER_JUMP_FORCE (PLAYER_FORCE*10.f)

#define PLAYER_STATE_SHOW_INVENTORY 0b0000'0000'0000'0001

/// Items
#define SMALL_ITEM_SIZE (TILE_SIZE/M_PI)
#define SMALL_ITEM_EXTENTS glm::vec2(SMALL_ITEM_SIZE/2.f)
#define ITEM_ICON_SIZE (SMALL_ITEM_SIZE*0.9f)
#define ITEM_ICON_DIMS glm::vec2(ITEM_ICON_SIZE)

/// Rendering
#define DEFAULT_CAMERA_CENTER glm::vec2(PLAYER_INIT_POS.x, PLAYER_INIT_POS.y)
#define DEFAULT_CAMERA_DIMENSIONS glm::vec2(TILE_SIZE*3.f)

#define DEBUG_BOX2D_COLOR glm::vec4(0.5,0,0.3,1)
#define DEBUG_BOX2D_AABB_COLOR glm::vec4(1,0,0,1)

/// Text
#define MEDIUM_TEXT_HEIGHT 5

/// Timing
#define STANDARD_POPUP_TIME TARGET_FPS
#define HOLD_TIME (TARGET_FPS*0.75)

#endif // DEFS_H_INCLUDED