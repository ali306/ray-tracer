// test_groups.c

#include "../include/shapes.h"
#include "../include/transformations.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void test_groups(void)
{
    { // Creating a new group

        group_t *g = group();
        assert(g != NULL);
        assert(matrix_equal(g->transform, IDENTITY));
        assert(g->child_count == 0);
        group_free(g);
    }

    { // A shape has a parent attribute
        sphere_t s = sphere();
        assert(s.parent == NULL);
    }

    { // Adding a child to a group
        group_t *g      = group();
        test_shape_t *s = malloc(sizeof(test_shape_t));
        *s              = test_shape();

        group_add_child(g, (shape_t *)s);

        assert(g->child_count > 0);
        assert(group_includes(g, (shape_t *)s));
        assert(s->parent == g);

        group_free(g);
    }

    { // Intersecting a ray with an empty group
        group_t *g         = group();
        ray_t r            = ray(point(0, 0, 0), vector(0, 0, 1));
        intersections_t xs = group_intersect(g, r);
        assert(xs.count == 0);
        group_free(g);
    }

    { // Intersecting a ray with a nonempty group
        group_t *g = group();

        sphere_t *s1 = malloc(sizeof(sphere_t));
        *s1          = sphere();

        sphere_t *s2 = malloc(sizeof(sphere_t));
        *s2          = sphere();
        shape_set_transform((shape_t *)s2, transform_translation(0, 0, -3));

        sphere_t *s3 = malloc(sizeof(sphere_t));
        *s3          = sphere();
        shape_set_transform((shape_t *)s3, transform_translation(5, 0, 0));

        group_add_child(g, (shape_t *)s1);
        group_add_child(g, (shape_t *)s2);
        group_add_child(g, (shape_t *)s3);

        ray_t r            = ray(point(0, 0, -5), vector(0, 0, 1));
        intersections_t xs = group_intersect(g, r);

        assert(xs.count == 4);
        assert(xs.intersections[0].object == s2);
        assert(xs.intersections[1].object == s2);
        assert(xs.intersections[2].object == s1);
        assert(xs.intersections[3].object == s1);

        group_free(g);
    }

    { // Intersecting a transformed group
        group_t *g = group();
        shape_set_transform((shape_t *)g, transform_scaling(2, 2, 2));

        sphere_t *s = malloc(sizeof(sphere_t));
        *s          = sphere();
        shape_set_transform((shape_t *)s, transform_translation(5, 0, 0));

        group_add_child(g, (shape_t *)s);

        ray_t r            = ray(point(10, 0, -10), vector(0, 0, 1));
        intersections_t xs = shape_intersect((shape_t *)g, r);

        assert(xs.count == 2);

        group_free(g);
    }

    { // Converting a point from world to object space
        group_t *g1 = group();
        shape_set_transform((shape_t *)g1, transform_rotation_y((M_PI / 2)));

        group_t *g2 = group();
        shape_set_transform((shape_t *)g2, transform_scaling(2, 2, 2));
        group_add_child(g1, (shape_t *)g2);

        sphere_t *s = malloc(sizeof(sphere_t));
        *s          = sphere();
        shape_set_transform((shape_t *)s, transform_translation(5, 0, 0));
        group_add_child(g2, (shape_t *)s);

        tuple_t p = world_to_object((shape_t *)s, point(-2, 0, -10));

        assert(tuple_equal(p, point(0, 0, -1)));

        group_free(g1);
    }

    { // Converting a normal from object to world space

        group_t *g1 = group();
        shape_set_transform((shape_t *)g1, transform_rotation_y((M_PI / 2)));

        group_t *g2 = group();
        shape_set_transform((shape_t *)g2, transform_scaling(1, 2, 3));
        group_add_child(g1, (shape_t *)g2);

        sphere_t *s = malloc(sizeof(sphere_t));
        *s          = sphere();
        shape_set_transform((shape_t *)s, transform_translation(5, 0, 0));
        group_add_child(g2, (shape_t *)s);

        double sqrt3_div3 = sqrt(3.0) / 3.0;
        tuple_t n         = normal_to_world((shape_t *)s,
                                            vector(sqrt3_div3, sqrt3_div3, sqrt3_div3));

        assert(fabs(n.x - 0.2857) < 0.001);
        assert(fabs(n.y - 0.4286) < 0.001);
        assert(fabs(n.z - (-0.8571)) < 0.001);
        group_free(g1);
    }

    { // Finding the normal on a child object

        group_t *g1 = group();
        shape_set_transform((shape_t *)g1, transform_rotation_y((M_PI / 2)));

        group_t *g2 = group();
        shape_set_transform((shape_t *)g2, transform_scaling(1, 2, 3));
        group_add_child(g1, (shape_t *)g2);

        sphere_t *s = malloc(sizeof(sphere_t));
        *s          = sphere();
        shape_set_transform((shape_t *)s, transform_translation(5, 0, 0));
        group_add_child(g2, (shape_t *)s);

        tuple_t n =
            shape_normal_at((shape_t *)s, point(1.7321, 1.1547, -5.5774), NULL);

        assert(fabs(n.x - 0.2857) < 0.001);
        assert(fabs(n.y - 0.4286) < 0.001);
        assert(fabs(n.z - (-0.8571)) < 0.001);
        group_free(g1);
    }

    { // Bounding box optimization - ray misses box

        test_shape_t *child = malloc(sizeof(test_shape_t));
        *child              = test_shape();
        group_t *shape      = group();
        group_add_child(shape, (shape_t *)child);
        ray_t r = ray(point(0, 0, -5), vector(0, 1, 0));

        intersections_t xs = shape_intersect((shape_t *)shape, r);
        (void)xs;

        assert(child->has_saved_ray == false);

        group_free(shape);
    }

    { // Bounding box optimization - ray hits box

        test_shape_t *child = malloc(sizeof(test_shape_t));
        *child              = test_shape();
        group_t *shape      = group();
        group_add_child(shape, (shape_t *)child);
        ray_t r = ray(point(0, 0, -5), vector(0, 0, 1));

        intersections_t xs = shape_intersect((shape_t *)shape, r);
        (void)xs;

        assert(child->has_saved_ray == true);

        group_free(shape);
    }

    { // Partitioning children based on bounding box splits

        sphere_t *s1 = malloc(sizeof(sphere_t));
        *s1          = sphere();
        sphere_set_transform(s1, transform_translation(-2, 0, 0));

        sphere_t *s2 = malloc(sizeof(sphere_t));
        *s2          = sphere();
        sphere_set_transform(s2, transform_translation(2, 0, 0));

        sphere_t *s3 = malloc(sizeof(sphere_t));
        *s3          = sphere();

        group_t *g = group();
        group_add_child(g, (shape_t *)s1);
        group_add_child(g, (shape_t *)s2);
        group_add_child(g, (shape_t *)s3);

        shape_list_t left  = shape_list_create();
        shape_list_t right = shape_list_create();
        partition_children(g, &left, &right);

        assert(g->child_count == 1);
        assert(g->children[0] == (shape_t *)s3);

        assert(left.count == 1);
        assert(left.shapes[0] == (shape_t *)s1);

        assert(right.count == 1);
        assert(right.shapes[0] == (shape_t *)s2);

        free(s1);
        free(s2);

        shape_list_free(&left);
        shape_list_free(&right);
        group_free(g);
    }

    { // Creating subgroups

        sphere_t *s1 = malloc(sizeof(sphere_t));
        sphere_t *s2 = malloc(sizeof(sphere_t));
        *s1          = sphere();
        *s2          = sphere();

        group_t *g = group();

        shape_list_t children = shape_list_create();
        shape_list_add(&children, (shape_t *)s1);
        shape_list_add(&children, (shape_t *)s2);

        make_subgroup(g, &children);

        assert(g->child_count == 1);
        assert(g->children[0]->type == SHAPE_GROUP);

        group_t *subgroup = (group_t *)g->children[0];
        assert(subgroup->child_count == 2);
        assert(subgroup->children[0] == (shape_t *)s1);
        assert(subgroup->children[1] == (shape_t *)s2);

        shape_list_free(&children);
        group_free(g);
    }

    { // Subdividing a primitive does nothing

        sphere_t shape = sphere();
        divide((shape_t *)&shape, 1);

        assert(shape.type == SHAPE_SPHERE);
    }

    { // Subdividing a group partitions its children
        sphere_t *s1 = malloc(sizeof(sphere_t));
        sphere_t *s2 = malloc(sizeof(sphere_t));
        sphere_t *s3 = malloc(sizeof(sphere_t));

        *s1 = sphere();
        shape_set_transform((shape_t *)s1, transform_translation(-2, -2, 0));

        *s2 = sphere();
        shape_set_transform((shape_t *)s2, transform_translation(-2, 2, 0));

        *s3 = sphere();
        shape_set_transform((shape_t *)s3, transform_scaling(4, 4, 4));

        group_t *g = group();
        group_add_child(g, (shape_t *)s1);
        group_add_child(g, (shape_t *)s2);
        group_add_child(g, (shape_t *)s3);

        divide((shape_t *)g, 1);

        assert(g->children[0] == (shape_t *)s3);

        assert(g->child_count == 2);
        assert(g->children[1]->type == SHAPE_GROUP);

        group_t *subgroup = (group_t *)g->children[1];
        assert(subgroup->child_count == 2);

        assert((subgroup->children[0]->type == SHAPE_GROUP &&
                subgroup->children[1]->type == SHAPE_GROUP) ||
               (subgroup->children[0] == (shape_t *)s1 ||
                subgroup->children[0] == (shape_t *)s2));

        group_free(g);
    }

    { // Subdividing a group with too few children
        sphere_t *s1 = malloc(sizeof(sphere_t));
        sphere_t *s2 = malloc(sizeof(sphere_t));
        sphere_t *s3 = malloc(sizeof(sphere_t));
        sphere_t *s4 = malloc(sizeof(sphere_t));

        *s1 = sphere();
        shape_set_transform((shape_t *)s1, transform_translation(-2, 0, 0));

        *s2 = sphere();
        shape_set_transform((shape_t *)s2, transform_translation(2, 1, 0));

        *s3 = sphere();
        shape_set_transform((shape_t *)s3, transform_translation(2, -1, 0));

        *s4 = sphere();

        group_t *subgroup = group();
        group_add_child(subgroup, (shape_t *)s1);
        group_add_child(subgroup, (shape_t *)s2);
        group_add_child(subgroup, (shape_t *)s3);

        group_t *g = group();
        group_add_child(g, (shape_t *)subgroup);
        group_add_child(g, (shape_t *)s4);

        divide((shape_t *)g, 3);

        assert(g->children[0] == (shape_t *)subgroup);
        assert(g->children[1] == (shape_t *)s4);

        assert(subgroup->child_count == 2);
        assert(subgroup->children[0]->type == SHAPE_GROUP);
        assert(subgroup->children[1]->type == SHAPE_GROUP);

        group_t *sub_left  = (group_t *)subgroup->children[0];
        group_t *sub_right = (group_t *)subgroup->children[1];

        assert((sub_left->child_count == 1 && sub_right->child_count == 2) ||
               (sub_left->child_count == 2 && sub_right->child_count == 1));

        group_free(g);
    }
}

int main(void)
{
    test_groups();
    return 0;
}