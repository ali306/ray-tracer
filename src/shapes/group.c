#include "../../include/bounds.h"
#include "../../include/dynamic_array.h"
#include "../../include/shapes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

group_t *group(void)
{
    group_t *g = malloc(sizeof(group_t));
    if (g == NULL)
    {
        return NULL;
    }

    g->type                         = SHAPE_GROUP;
    g->transform                    = IDENTITY;
    g->inverse_transform            = IDENTITY;
    g->transposed_inverse_transform = IDENTITY;
    g->material                     = material();
    g->parent                       = NULL;

    g->child_count       = 0;
    g->children_capacity = 16;
    g->children          = calloc(g->children_capacity, sizeof(shape_t *));

    if (g->children == NULL)
    {
        free(g);
        return NULL;
    }

    g->bounds_cached     = false;
    g->cached_bounds_min = point(0, 0, 0);
    g->cached_bounds_max = point(0, 0, 0);

    g->world_bounds        = bounding_box_empty();
    g->world_bounds_cached = false;

    return g;
}

static bool group_ensure_capacity(group_t *g)
{
    if (g == NULL)
    {
        return false;
    }

    DYN_ARRAY_ENSURE_CAPACITY_IMPL(g->children, g->child_count,
                                   g->children_capacity, shape_t *,
                                   MAX_GROUP_CHILDREN);
    return true;
}

static void group_invalidate_bounds_cache(group_t *g)
{
    while (g != NULL)
    {
        g->bounds_cached = false;
        if (g->parent != NULL && ((shape_t *)g->parent)->type == SHAPE_GROUP)
        {
            g = (group_t *)g->parent;
        }
        else
        {
            break;
        }
    }
}

void group_add_child(group_t *g, shape_t *s)
{
    if (s == NULL || !group_ensure_capacity(g))
    {
        return;
    }

    g->children[g->child_count] = s;
    s->parent                   = g;
    g->child_count++;

    group_invalidate_bounds_cache(g);
}

bool group_includes(const group_t *g, const shape_t *s)
{
    if (g == NULL || s == NULL)
    {
        return false;
    }

    for (unsigned i = 0; i < g->child_count; i++)
    {
        if (g->children[i] == s)
        {
            return true;
        }
    }
    return false;
}

intersections_t group_local_intersect(const group_t *g, ray_t r)
{
    if (g == NULL)
    {
        return empty_intersections();
    }

    intersections_t result;
    result.count = 0;

    if (g->child_count > 0)
    {
        bounding_box_t group_bounds = bounds_of_group(g);
        if (!bounds_intersects(group_bounds, r))
        {
            return result;
        }
    }

    for (unsigned i = 0; i < g->child_count; i++)
    {

        intersections_t child_xs = shape_intersect(g->children[i], r);

        for (int j = 0; j < child_xs.count; j++)
        {
            if (result.count < MAX_INTERSECTIONS)
            {
                result.intersections[result.count] = child_xs.intersections[j];
                result.count++;
            }
            else
            {
                static int overflow_warned = 0;
                if (!overflow_warned)
                {
                    printf("WARNING: MAX_INTERSECTIONS overflow in "
                           "group_local_intersect\n");
                    overflow_warned = 1;
                }
            }
        }
    }

    if (result.count > 1)
    {
        intersections_sort(&result);
    }

    return result;
}

intersections_t group_intersect(const group_t *g, ray_t r)
{
    return group_local_intersect(g, r);
}

void group_free(group_t *g)
{
    if (g == NULL)
    {
        return;
    }

    if (g->children != NULL)
    {
        for (unsigned i = 0; i < g->child_count; i++)
        {
            if (g->children[i] != NULL)
            {
                shape_t *child = g->children[i];
                child->parent  = NULL;

                if (child->type == SHAPE_GROUP)
                {
                    group_free((group_t *)child);
                }
                else
                {
                    free(child);
                }
            }
        }
        free(g->children);
        g->children = NULL;
    }

    g->child_count       = 0;
    g->children_capacity = 0;

    free(g);
}

shape_list_t shape_list_create(void)
{
    shape_list_t list;
    list.capacity = 16;
    list.count    = 0;
    list.shapes   = calloc(list.capacity, sizeof(shape_t *));
    if (list.shapes == NULL)
    {
        list.capacity = 0;
    }
    return list;
}

void shape_list_free(shape_list_t *list)
{
    if (list == NULL)
    {
        return;
    }

    if (list->shapes != NULL)
    {
        free(list->shapes);
        list->shapes = NULL;
    }
    list->count    = 0;
    list->capacity = 0;
}

static bool shape_list_ensure_capacity(shape_list_t *list)
{
    if (list == NULL)
    {
        return false;
    }

    DYN_ARRAY_ENSURE_CAPACITY_IMPL(list->shapes, list->count, list->capacity,
                                   shape_t *, MAX_GROUP_CHILDREN);
    return true;
}

void shape_list_add(shape_list_t *list, shape_t *shape)
{
    if (shape == NULL || !shape_list_ensure_capacity(list))
    {
        return;
    }

    list->shapes[list->count] = shape;
    list->count++;
}

void partition_children(group_t *group, shape_list_t *left, shape_list_t *right)
{
    if (group == NULL || left == NULL || right == NULL)
        return;

    bounding_box_t group_bounds = bounds_of_group(group);
    bounding_box_t left_bounds, right_bounds;
    split_bounds(group_bounds, &left_bounds, &right_bounds);

    shape_t **remaining = calloc(group->child_count, sizeof(shape_t *));
    if (remaining == NULL)
        return;
    unsigned remaining_count = 0;

    for (unsigned i = 0; i < group->child_count; i++)
    {
        if (group->children[i] != NULL)
        {

            bounding_box_t child_bounds =
                bounds_parent_space_bounds_of(group->children[i]);

            if (bounds_box_contains_box(left_bounds, child_bounds))
            {
                shape_list_add(left, group->children[i]);
                group->children[i]->parent = NULL;
            }

            else if (bounds_box_contains_box(right_bounds, child_bounds))
            {
                shape_list_add(right, group->children[i]);
                group->children[i]->parent = NULL;
            }

            else
            {
                remaining[remaining_count] = group->children[i];
                remaining_count++;
            }
        }
    }

    for (unsigned i = 0; i < remaining_count; i++)
    {
        group->children[i] = remaining[i];
    }
    group->child_count = remaining_count;

    for (unsigned i = remaining_count; i < group->children_capacity; i++)
    {
        group->children[i] = NULL;
    }

    free(remaining);
}

void make_subgroup(group_t *parent_group, shape_list_t *children)
{
    if (parent_group == NULL || children == NULL || children->count == 0)
        return;

    group_t *subgroup = group();
    if (subgroup == NULL)
    {
        for (unsigned i = 0; i < children->count; i++)
        {
            if (children->shapes[i] != NULL)
            {
                if (children->shapes[i]->type == SHAPE_GROUP)
                {
                    group_free((group_t *)children->shapes[i]);
                }
                else
                {
                    free(children->shapes[i]);
                }
            }
        }
        return;
    }

    for (unsigned i = 0; i < children->count; i++)
    {
        if (children->shapes[i] != NULL)
        {
            group_add_child(subgroup, children->shapes[i]);
        }
    }

    if (subgroup->child_count > 0)
    {
        group_add_child(parent_group, (shape_t *)subgroup);
    }
    else
    {

        group_free(subgroup);
    }
}

void divide(shape_t *shape, unsigned threshold)
{
    if (shape == NULL)
    {
        return;
    }

    divide_recursive(shape, threshold, 10);
}

void divide_recursive(shape_t *shape, unsigned threshold, unsigned max_depth)
{
    if (shape == NULL || max_depth == 0)
        return;

    if (shape->type != SHAPE_GROUP)
    {
        return;
    }

    group_t *group = (group_t *)shape;

    if (threshold <= group->child_count)
    {
        shape_list_t left  = shape_list_create();
        shape_list_t right = shape_list_create();

        partition_children(group, &left, &right);

        if (left.count > 0)
        {
            make_subgroup(group, &left);
        }
        if (right.count > 0)
        {
            make_subgroup(group, &right);
        }

        shape_list_free(&left);
        shape_list_free(&right);
    }

    for (unsigned i = 0; i < group->child_count; i++)
    {
        if (group->children[i] != NULL)
        {
            divide_recursive(group->children[i], threshold, max_depth - 1);
        }
    }
}