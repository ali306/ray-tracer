#include "../include/obj_parser.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void smooth_fan_triangulation(group_t *group, int vertex_indices[],
                                     int normal_indices[], int vertex_count,
                                     obj_parser_t *parser)
{
    if (group == NULL || vertex_indices == NULL || normal_indices == NULL ||
        parser == NULL || vertex_count < 3)
    {
        return;
    }

    for (int index = 1; index < vertex_count - 1; index++)
    {
        if (vertex_indices[0] < 1 ||
            vertex_indices[0] > (int)parser->vertex_count ||
            vertex_indices[index] < 1 ||
            vertex_indices[index] > (int)parser->vertex_count ||
            vertex_indices[index + 1] < 1 ||
            vertex_indices[index + 1] > (int)parser->vertex_count)
        {
            continue;
        }

        if (normal_indices[0] < 1 ||
            normal_indices[0] > (int)parser->normal_count ||
            normal_indices[index] < 1 ||
            normal_indices[index] > (int)parser->normal_count ||
            normal_indices[index + 1] < 1 ||
            normal_indices[index + 1] > (int)parser->normal_count)
        {
            continue;
        }

        tuple_t p1 = parser->vertices[vertex_indices[0]];
        tuple_t p2 = parser->vertices[vertex_indices[index]];
        tuple_t p3 = parser->vertices[vertex_indices[index + 1]];

        tuple_t n1 = parser->normals[normal_indices[0]];
        tuple_t n2 = parser->normals[normal_indices[index]];
        tuple_t n3 = parser->normals[normal_indices[index + 1]];

        smooth_triangle_t *t = malloc(sizeof(smooth_triangle_t));
        if (t == NULL)
        {
            printf("ERROR: Failed to allocate memory for smooth triangle\n");
            continue;
        }
        *t = smooth_triangle(p1, p2, p3, n1, n2, n3);

        unsigned old_count = group->child_count;
        group_add_child(group, (shape_t *)t);
        if (group->child_count == old_count)
        {
            printf("ERROR: Failed to add smooth triangle to group (capacity "
                   "exceeded)\n");
            free(t);
            continue;
        }
    }
}

static void fan_triangulation(group_t *group, tuple_t vertices[],
                              int vertex_count)
{
    if (group == NULL || vertices == NULL || vertex_count < 3)
    {
        return;
    }

    for (int index = 1; index < vertex_count - 1; index++)
    {
        triangle_t *t = malloc(sizeof(triangle_t));
        if (t == NULL)
        {
            printf("ERROR: Failed to allocate memory for triangle\n");
            continue;
        }
        *t = triangle(vertices[0], vertices[index], vertices[index + 1]);

        unsigned old_count = group->child_count;
        group_add_child(group, (shape_t *)t);
        if (group->child_count == old_count)
        {
            printf(
                "ERROR: Failed to add triangle to group (capacity exceeded)\n");
            free(t);
            continue;
        }
    }
}

static void parse_vertex_normal(obj_parser_t *parser, const char *line)
{
    if (parser == NULL || line == NULL)
    {
        return;
    }

    double x, y, z;
    if (sscanf(line + 3, "%lf %lf %lf", &x, &y, &z) == 3)
    {
        if (parser->normal_count < MAX_NORMALS)
        {
            parser->normal_count++;
            parser->normals[parser->normal_count] = vector(x, y, z);
        }
        else
        {
            parser->ignored_lines++;
        }
    }
    else
    {
        parser->ignored_lines++;
    }
}

static void parse_vertex(obj_parser_t *parser, const char *line)
{
    if (parser == NULL || line == NULL)
    {
        return;
    }

    double x, y, z;
    if (sscanf(line + 2, "%lf %lf %lf", &x, &y, &z) == 3)
    {
        if (parser->vertex_count < MAX_VERTICES)
        {
            parser->vertex_count++;
            parser->vertices[parser->vertex_count] = point(x, y, z);
        }
        else
        {
            parser->ignored_lines++;
        }
    }
    else
    {
        parser->ignored_lines++;
    }
}

static void parse_face(obj_parser_t *parser, const char *line)
{
    if (parser == NULL || line == NULL)
    {
        return;
    }

    int face_vertices[MAX_FACE_VERTICES];
    int face_normals[MAX_FACE_VERTICES];
    int vertex_count = 0;
    bool has_normals = false;
    char *line_ptr   = (char *)line + 2;
    char *token;

    char line_copy[256];
    strncpy(line_copy, line_ptr, sizeof(line_copy) - 1);
    line_copy[sizeof(line_copy) - 1] = '\0';

    char *line_work = line_copy;
    while ((token = strtok(vertex_count == 0 ? line_work : NULL, " ")) !=
               NULL &&
           vertex_count < MAX_FACE_VERTICES)
    {
        int vertex_index = 0;
        int normal_index = 0;

        char token_copy[64];
        strncpy(token_copy, token, sizeof(token_copy) - 1);
        token_copy[sizeof(token_copy) - 1] = '\0';

        char *slash1 = strchr(token_copy, '/');
        if (slash1 == NULL)
        {
            vertex_index = atoi(token_copy);
        }
        else
        {
            *slash1      = '\0';
            vertex_index = atoi(token_copy);

            char *slash2 = strchr(slash1 + 1, '/');
            if (slash2 != NULL)
            {
                normal_index = atoi(slash2 + 1);
                if (normal_index > 0)
                {
                    has_normals = true;
                }
            }
        }

        if (vertex_index > 0 && vertex_index <= (int)parser->vertex_count)
        {
            face_vertices[vertex_count] = vertex_index;
            face_normals[vertex_count]  = normal_index;
            vertex_count++;
        }
        else
        {
            vertex_count = 0;
            break;
        }
    }

    if (vertex_count >= 3)
    {
        if (has_normals)
        {
            smooth_fan_triangulation(parser->current_group, face_vertices,
                                     face_normals, vertex_count, parser);
        }
        else
        {
            tuple_t face_points[MAX_FACE_VERTICES];
            for (int i = 0; i < vertex_count; i++)
            {
                face_points[i] = parser->vertices[face_vertices[i]];
            }
            fan_triangulation(parser->current_group, face_points, vertex_count);
        }
    }
    else
    {
        parser->ignored_lines++;
    }
}

static void parse_group(obj_parser_t *parser, const char *line)
{
    if (parser == NULL || line == NULL)
    {
        return;
    }

    char group_name[MAX_GROUP_NAME];
    if (sscanf(line + 2, "%63s", group_name) == 1)
    {
        group_t *found_group = NULL;
        for (unsigned i = 0; i < parser->group_count; i++)
        {
            if (strcmp(parser->named_groups[i].name, group_name) == 0)
            {
                found_group = parser->named_groups[i].group;
                break;
            }
        }

        if (found_group == NULL && parser->group_count < MAX_GROUPS)
        {
            group_t *new_group = group();
            if (new_group != NULL)
            {
                strncpy(parser->named_groups[parser->group_count].name,
                        group_name, MAX_GROUP_NAME - 1);
                parser->named_groups[parser->group_count]
                    .name[MAX_GROUP_NAME - 1]                   = '\0';
                parser->named_groups[parser->group_count].group = new_group;
                found_group                                     = new_group;
                parser->group_count++;
            }
        }

        if (found_group != NULL)
        {
            parser->current_group = found_group;
        }
        else
        {
            parser->ignored_lines++;
        }
    }
    else
    {
        parser->ignored_lines++;
    }
}

obj_parser_t obj_parse_file(FILE *file)
{
    obj_parser_t parser = {0};

    if (file == NULL)
    {
        parser.has_error = true;
        return parser;
    }

    parser.default_group = group();
    if (parser.default_group == NULL)
    {
        parser.has_error = true;
        return parser;
    }
    parser.current_group = parser.default_group;
    parser.has_error     = false;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[sizeof(line) - 1]    = '\0';
        line[strcspn(line, "\n")] = 0;

        if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            parse_vertex_normal(&parser, line);
        }
        else if (line[0] == 'v' && line[1] == ' ')
        {
            parse_vertex(&parser, line);
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            parse_face(&parser, line);
        }
        else if (line[0] == 'g' && line[1] == ' ')
        {
            parse_group(&parser, line);
        }
        else
        {
            parser.ignored_lines++;
        }
    }

    return parser;
}

group_t *obj_parser_get_group(obj_parser_t *parser, const char *name)
{
    if (parser == NULL || name == NULL)
    {
        return NULL;
    }

    for (unsigned i = 0; i < parser->group_count; i++)
    {
        if (strcmp(parser->named_groups[i].name, name) == 0)
        {
            return parser->named_groups[i].group;
        }
    }
    return NULL;
}

group_t *obj_parser_get_default_group(obj_parser_t *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    return parser->default_group;
}

bool obj_parser_has_error(const obj_parser_t *parser)
{
    return parser != NULL && parser->has_error;
}

void obj_parser_free(obj_parser_t *parser)
{
    if (parser == NULL)
        return;

    if (parser->default_group != NULL)
    {
        group_free(parser->default_group);
        parser->default_group = NULL;
    }

    for (unsigned int i = 0; i < parser->group_count; i++)
    {
        if (parser->named_groups[i].group != NULL)
        {
            group_free(parser->named_groups[i].group);
            parser->named_groups[i].group = NULL;
        }
    }
    parser->group_count = 0;
    parser->has_error   = false;
}