// config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <math.h>

// ===== MATHEMATICAL CONSTANTS =====

#define EPSILON 1e-4

#define M_PI_3 (M_PI / 3)
#define M_PI_5 (M_PI / 5)
// ===== MEMORY LIMITS =====

#define MAX_NUM_OBJECTS 100000

#define MAX_GROUP_CHILDREN 50000

#define MAX_INTERSECTIONS 64

#define MAX_SEQUENCE_LENGTH 32

// ===== OBJ FILE PARSER LIMITS =====

#define MAX_VERTICES   50000
#define MAX_NORMALS    50000
#define MAX_GROUPS     50
#define MAX_GROUP_NAME 64

#define MAX_FACE_VERTICES 10

// ===== RENDERING CONFIGURATION =====

#define MAX_RECURSION 5

#define MIN_RAY_CONTRIBUTION 0.01

#define DEFAULT_SCENE_WIDTH 1000

// ===== COLOR CONSTANTS =====

#define BLACK color(0, 0, 0)
#define WHITE color(1, 1, 1)

#endif