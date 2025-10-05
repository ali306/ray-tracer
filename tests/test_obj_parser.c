// test_obj_parser.c

#include "../include/obj_parser.h"
#include <assert.h>
#include <string.h>

void test_obj_parser(void)
{
    { // Ignoring unrecognized lines
        FILE *gibberish = tmpfile();
        fprintf(gibberish, "There was a young lady named Bright\n");
        fprintf(gibberish, "who traveled much faster than light.\n");
        fprintf(gibberish, "She set out one day\n");
        fprintf(gibberish, "in a relative way,\n");
        fprintf(gibberish, "and came back the previous night.\n");
        rewind(gibberish);

        obj_parser_t parser = obj_parse_file(gibberish);
        assert(parser.ignored_lines == 5);

        obj_parser_free(&parser);
        fclose(gibberish);
    }

    { // Vertex records
        FILE *file = tmpfile();
        fprintf(file, "v -1 1 0\n");
        fprintf(file, "v -1.0000 0.5000 0.0000\n");
        fprintf(file, "v 1 0 0\n");
        fprintf(file, "v 1 1 0\n");
        rewind(file);

        obj_parser_t parser = obj_parse_file(file);
        assert(tuple_equal(parser.vertices[1], point(-1, 1, 0)));
        assert(tuple_equal(parser.vertices[2], point(-1, 0.5, 0)));
        assert(tuple_equal(parser.vertices[3], point(1, 0, 0)));
        assert(tuple_equal(parser.vertices[4], point(1, 1, 0)));

        obj_parser_free(&parser);
        fclose(file);
    }

    { // Parsing triangle faces
        FILE *file = tmpfile();
        fprintf(file, "v -1 1 0\n");
        fprintf(file, "v -1 0 0\n");
        fprintf(file, "v 1 0 0\n");
        fprintf(file, "v 1 1 0\n");
        fprintf(file, "f 1 2 3\n");
        fprintf(file, "f 1 3 4\n");
        rewind(file);

        obj_parser_t parser = obj_parse_file(file);
        group_t *g          = obj_parser_get_default_group(&parser);
        triangle_t *t1      = (triangle_t *)g->children[0];
        triangle_t *t2      = (triangle_t *)g->children[1];

        assert(tuple_equal(t1->p1, parser.vertices[1]));
        assert(tuple_equal(t1->p2, parser.vertices[2]));
        assert(tuple_equal(t1->p3, parser.vertices[3]));
        assert(tuple_equal(t2->p1, parser.vertices[1]));
        assert(tuple_equal(t2->p2, parser.vertices[3]));
        assert(tuple_equal(t2->p3, parser.vertices[4]));

        obj_parser_free(&parser);
        fclose(file);
    }

    { // Triangulating polygons
        FILE *file = tmpfile();
        fprintf(file, "v -1 1 0\n");
        fprintf(file, "v -1 0 0\n");
        fprintf(file, "v 1 0 0\n");
        fprintf(file, "v 1 1 0\n");
        fprintf(file, "v 0 2 0\n");
        fprintf(file, "f 1 2 3 4 5\n");
        rewind(file);

        obj_parser_t parser = obj_parse_file(file);
        group_t *g          = obj_parser_get_default_group(&parser);
        triangle_t *t1      = (triangle_t *)g->children[0];
        triangle_t *t2      = (triangle_t *)g->children[1];
        triangle_t *t3      = (triangle_t *)g->children[2];

        assert(tuple_equal(t1->p1, parser.vertices[1]));
        assert(tuple_equal(t1->p2, parser.vertices[2]));
        assert(tuple_equal(t1->p3, parser.vertices[3]));
        assert(tuple_equal(t2->p1, parser.vertices[1]));
        assert(tuple_equal(t2->p2, parser.vertices[3]));
        assert(tuple_equal(t2->p3, parser.vertices[4]));
        assert(tuple_equal(t3->p1, parser.vertices[1]));
        assert(tuple_equal(t3->p2, parser.vertices[4]));
        assert(tuple_equal(t3->p3, parser.vertices[5]));

        obj_parser_free(&parser);
        fclose(file);
    }

    { // Triangles in groups
        FILE *file = fopen("../../tests/triangles.obj", "r");
        assert(file != NULL);

        obj_parser_t parser = obj_parse_file(file);
        group_t *g1         = obj_parser_get_group(&parser, "FirstGroup");
        group_t *g2         = obj_parser_get_group(&parser, "SecondGroup");

        assert(g1 != NULL);
        assert(g2 != NULL);

        triangle_t *t1 = (triangle_t *)g1->children[0];
        triangle_t *t2 = (triangle_t *)g2->children[0];

        assert(tuple_equal(t1->p1, parser.vertices[1]));
        assert(tuple_equal(t1->p2, parser.vertices[2]));
        assert(tuple_equal(t1->p3, parser.vertices[3]));
        assert(tuple_equal(t2->p1, parser.vertices[1]));
        assert(tuple_equal(t2->p2, parser.vertices[3]));
        assert(tuple_equal(t2->p3, parser.vertices[4]));

        obj_parser_free(&parser);
        fclose(file);
    }

    { // Converting an OBJ file to a group
        FILE *file = fopen("../../tests/triangles.obj", "r");
        if (file == NULL)
        {
            fprintf(stderr, "Failed to open ../../tests/triangles.obj\n");
            file = fopen("../../tests/triangles.obj", "r");
            if (file == NULL)
            {
                fprintf(stderr, "Failed to open ../../tests/triangles.obj\n");
                assert(0 && "Could not open triangles.obj from any path");
            }
        }

        obj_parser_t parser = obj_parse_file(file);

        group_t *first_group  = obj_parser_get_group(&parser, "FirstGroup");
        group_t *second_group = obj_parser_get_group(&parser, "SecondGroup");

        if (first_group == NULL)
        {
            fprintf(stderr, "FirstGroup not found. Group count: %d\n",
                    parser.group_count);
            for (unsigned i = 0; i < parser.group_count; i++)
            {
                fprintf(stderr, "Group %d: '%s'\n", i,
                        parser.named_groups[i].name);
            }
        }

        assert(first_group != NULL && first_group->child_count == 1);
        assert(second_group != NULL && second_group->child_count == 1);

        group_t *default_group = obj_parser_get_default_group(&parser);
        assert(default_group->child_count == 0);

        triangle_t *t1 = (triangle_t *)first_group->children[0];
        triangle_t *t2 = (triangle_t *)second_group->children[0];
        assert(t1->type == SHAPE_TRIANGLE);
        assert(t2->type == SHAPE_TRIANGLE);

        obj_parser_free(&parser);
        fclose(file);
    }

    { // Vertex normal records
        FILE *file = tmpfile();
        fprintf(file, "vn 0 0 1\n");
        fprintf(file, "vn 0.707 0 -0.707\n");
        fprintf(file, "vn 1 2 3\n");
        rewind(file);

        obj_parser_t parser = obj_parse_file(file);
        assert(tuple_equal(parser.normals[1], vector(0, 0, 1)));
        assert(tuple_equal(parser.normals[2], vector(0.707, 0, -0.707)));
        assert(tuple_equal(parser.normals[3], vector(1, 2, 3)));

        obj_parser_free(&parser);
        fclose(file);
    }

    { // Faces with normals
        FILE *file = tmpfile();
        fprintf(file, "v 0 1 0\n");
        fprintf(file, "v -1 0 0\n");
        fprintf(file, "v 1 0 0\n");
        fprintf(file, "vn -1 0 0\n");
        fprintf(file, "vn 1 0 0\n");
        fprintf(file, "vn 0 1 0\n");
        fprintf(file, "f 1//3 2//1 3//2\n");
        fprintf(file, "f 1/0/3 2/102/1 3/14/2\n");
        rewind(file);

        obj_parser_t parser = obj_parse_file(file);
        group_t *g          = obj_parser_get_default_group(&parser);

        assert(g->child_count == 2);

        smooth_triangle_t *t1 = (smooth_triangle_t *)g->children[0];
        smooth_triangle_t *t2 = (smooth_triangle_t *)g->children[1];

        assert(tuple_equal(t1->p1, parser.vertices[1]));
        assert(tuple_equal(t1->p2, parser.vertices[2]));
        assert(tuple_equal(t1->p3, parser.vertices[3]));

        assert(tuple_equal(t1->n1, parser.normals[3]));
        assert(tuple_equal(t1->n2, parser.normals[1]));
        assert(tuple_equal(t1->n3, parser.normals[2]));

        assert(tuple_equal(t2->p1, t1->p1));
        assert(tuple_equal(t2->p2, t1->p2));
        assert(tuple_equal(t2->p3, t1->p3));
        assert(tuple_equal(t2->n1, t1->n1));
        assert(tuple_equal(t2->n2, t1->n2));
        assert(tuple_equal(t2->n3, t1->n3));

        obj_parser_free(&parser);
        fclose(file);
    }
}

int main(void)
{
    test_obj_parser();
    return 0;
}