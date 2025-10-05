// obj_parser.h

#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include "config.h"
#include "shapes.h"
#include "tuples.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    char name[MAX_GROUP_NAME];
    group_t *group;
} named_group_t;

typedef struct
{
    unsigned ignored_lines;
    tuple_t vertices[MAX_VERTICES + 1];
    unsigned vertex_count;
    tuple_t normals[MAX_NORMALS + 1];
    unsigned normal_count;
    group_t *default_group;
    named_group_t named_groups[MAX_GROUPS];
    unsigned group_count;
    group_t *current_group;
    bool has_error;
} obj_parser_t;

obj_parser_t obj_parse_file(FILE *file);

group_t *obj_parser_get_group(obj_parser_t *parser, const char *name);

group_t *obj_parser_get_default_group(obj_parser_t *parser);

bool obj_parser_has_error(const obj_parser_t *parser);

void obj_parser_free(obj_parser_t *parser);

#endif