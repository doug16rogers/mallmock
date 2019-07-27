/* Copyright (c) 2019 Doug Rogers under the Zero Clause BSD License. */
/* You are free to do whatever you want with this software. See LICENSE.txt. */

/*
 * Unit test program for read_file.h/.c.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cut.h"
#include "mallmock.h"
#include "read_file.h"

const char *g_program_name = "read_file_test"; /**< This program name; overwritten by argv[0]. */

typedef struct test_s {
    char filename[0x400];
    read_file_t *rf;
} test_t;

/* ------------------------------------------------------------------------- */
static cut_result_t test_init(test_t *test) {
    CUT_TEST_PASS();
}   /* test_init() */

/* ------------------------------------------------------------------------- */
static void clear_test_data(test_t *test) {
    if (test->filename[0] != 0) {
        unlink(test->filename);
        test->filename[0] = 0;
    }
    read_file_delete_null(&test->rf);
}   /* clear_test_data() */

/* ------------------------------------------------------------------------- */
static void test_exit(test_t *test) {
    mallmock_reset();
    clear_test_data(test);
}   /* test_exit() */

/* ------------------------------------------------------------------------- */
static cut_result_t test_read_file_degenerate(test_t *test) {
    CUT_ASSERT_NULL(test->rf = read_file_new(NULL));
    CUT_ASSERT_NULL(test->rf = read_file_new("/etc/bob/some/file/that/does/not/exist"));
    CUT_TEST_PASS();
}   /* test_read_file_degenerate() */

/* ------------------------------------------------------------------------- */
static cut_result_t create_test_file(test_t *test, const char *contents) {
    size_t size = 0;
    int fd = -1;

    assert(NULL != test);
    assert(NULL != contents);

    clear_test_data(test);
    strncpy(test->filename, "/tmp/read_file_test.XXXXXXXX", sizeof(test->filename));
    fd = mkstemp(test->filename);
    if (fd < 0) {
        return CUT_RESULT_ERROR;
    }
    size = strlen(contents);
    while (size > 0) {
        ssize_t bytes = write(fd, contents, size);
        if (bytes <= 0) {
            close(fd);
            return CUT_RESULT_ERROR;
        }
        contents += bytes;
        size -= bytes;
    }
    close(fd);
    return CUT_RESULT_PASS;
}   /* create_test_file() */

/* ------------------------------------------------------------------------- */
static cut_result_t test_read_file_simple(test_t *test) {
    const char *test_data =
        "There once was a man from Manhattan\n"
        "whose seat at the pub was all satin,\n"
        "he tried to be cool\n"
        "when a drunk took his stool,\n"
        "but instead his poor pants were all shat in!\n";
    const char *test_data_no_ending_newline =
        "When John Henry was a little baby\n"
        "Just a sittin' on hi mammy's knee,\n"
        "Said, \"The Big Bend Tunnel on that C&O Road\n"
        "Gonna be the death of me, Lord God, Going to be the death of me.\"";
    const char *test_data_empty_lines =
        "Tell me, O Muse, of the man of many devices, who wandered...\n"
        "\n"
        "\n"
        "...and sprang to the great threshold with the bow and the quiver...\n"
        "\n"
        "\n"
        "...in the likeness of Mentor both in form and in voice.\n";

    CUT_RETURN(create_test_file(test, test_data));
    CUT_ASSERT_NOT_NULL(test->rf = read_file_new(test->filename));
    CUT_ASSERT_STRING(test->filename, read_file_get_filename(test->rf));
    CUT_ASSERT_INT(5, read_file_get_line_count(test->rf));
    CUT_ASSERT_STRING("There once was a man from Manhattan\n", read_file_get_line(test->rf, 0));
    CUT_ASSERT_STRING("whose seat at the pub was all satin,\n", read_file_get_line(test->rf, 1));
    CUT_ASSERT_STRING("he tried to be cool\n", read_file_get_line(test->rf, 2));
    CUT_ASSERT_STRING("when a drunk took his stool,\n", read_file_get_line(test->rf, 3));
    CUT_ASSERT_STRING("but instead his poor pants were all shat in!\n", read_file_get_line(test->rf, 4));
    CUT_ASSERT_NULL(read_file_get_line(test->rf, 5));

    CUT_RETURN(create_test_file(test, test_data_no_ending_newline));
    CUT_ASSERT_NOT_NULL(test->rf = read_file_new(test->filename));
    CUT_ASSERT_STRING(test->filename, read_file_get_filename(test->rf));
    CUT_ASSERT_INT(4, read_file_get_line_count(test->rf));
    CUT_ASSERT_STRING("When John Henry was a little baby\n", read_file_get_line(test->rf, 0));
    CUT_ASSERT_STRING("Just a sittin' on hi mammy's knee,\n", read_file_get_line(test->rf, 1));
    CUT_ASSERT_STRING("Said, \"The Big Bend Tunnel on that C&O Road\n", read_file_get_line(test->rf, 2));
    CUT_ASSERT_STRING("Gonna be the death of me, Lord God, Going to be the death of me.\"", read_file_get_line(test->rf, 3));
    CUT_ASSERT_NULL(read_file_get_line(test->rf, 4));

    CUT_RETURN(create_test_file(test, test_data_empty_lines));
    CUT_ASSERT_NOT_NULL(test->rf = read_file_new(test->filename));
    CUT_ASSERT_STRING(test->filename, read_file_get_filename(test->rf));
    CUT_ASSERT_INT(7, read_file_get_line_count(test->rf));
    CUT_ASSERT_STRING("Tell me, O Muse, of the man of many devices, who wandered...\n",
                      read_file_get_line(test->rf, 0));
    CUT_ASSERT_STRING("\n", read_file_get_line(test->rf, 1));
    CUT_ASSERT_STRING("\n", read_file_get_line(test->rf, 2));
    CUT_ASSERT_STRING("...and sprang to the great threshold with the bow and the quiver...\n",
                      read_file_get_line(test->rf, 3));
    CUT_ASSERT_STRING("\n", read_file_get_line(test->rf, 4));
    CUT_ASSERT_STRING("\n", read_file_get_line(test->rf, 5));
    CUT_ASSERT_STRING("...in the likeness of Mentor both in form and in voice.\n",
                      read_file_get_line(test->rf, 6));
    CUT_ASSERT_NULL(read_file_get_line(test->rf, 7));

    CUT_TEST_PASS();
}   /* test_read_file_simple() */

/* ------------------------------------------------------------------------- */
static cut_result_t test_read_file_low_memory(test_t *test) {
    const char *test_data =
        "My dreams find comfort in the dark and weight of sloth;\n"
        "they revel in that warmth of inertia where they live\n"
        "until, forgotten, they rot and die.\n";

    CUT_RETURN(create_test_file(test, test_data));

    /* One for main rf object and one for each line. */
    mallmock_set_any_alloc_return(NULL, 0);
    CUT_ASSERT_NULL(test->rf = read_file_new(test->filename));

    mallmock_set_any_alloc_return(NULL, 1);
    CUT_ASSERT_NULL(test->rf = read_file_new(test->filename));

    mallmock_set_any_alloc_return(NULL, 2);
    CUT_ASSERT_NULL(test->rf = read_file_new(test->filename));

    mallmock_set_any_alloc_return(NULL, 3);
    CUT_ASSERT_NULL(test->rf = read_file_new(test->filename));

    mallmock_set_any_alloc_return(NULL, 4);
    CUT_ASSERT_NULL(test->rf = read_file_new(test->filename));

    mallmock_set_any_alloc_return(NULL, 5);
    CUT_ASSERT_NOT_NULL(test->rf = read_file_new(test->filename));
    mallmock_reset();
    CUT_ASSERT_STRING(test->filename, read_file_get_filename(test->rf));
    CUT_ASSERT_INT(3, read_file_get_line_count(test->rf));
    CUT_ASSERT_STRING("My dreams find comfort in the dark and weight of sloth;\n", read_file_get_line(test->rf, 0));
    CUT_ASSERT_STRING("they revel in that warmth of inertia where they live\n", read_file_get_line(test->rf, 1));
    CUT_ASSERT_STRING("until, forgotten, they rot and die.\n", read_file_get_line(test->rf, 2));
    CUT_ASSERT_NULL(read_file_get_line(test->rf, 4));
    CUT_TEST_PASS();
}   /* test_read_file_low_memory() */

/* ------------------------------------------------------------------------- */
void test_read_file(void) {
    CUT_CONFIG_SUITE(sizeof(test_t), test_init, test_exit);
    CUT_ADD_TEST(test_read_file_degenerate);
    CUT_ADD_TEST(test_read_file_simple);
    CUT_ADD_TEST(test_read_file_low_memory);
}   /* test_read_file() */

/* ------------------------------------------------------------------------- */
static void usage(FILE* f, int exit_code) CUT_GNU_ATTRIBUTE((noexit));
static void usage(FILE* f, int exit_code) {
    fprintf(f, "\n");
    fprintf(f, "Usage: %s [options] [test-substring...]\n", g_program_name);
    fprintf(f, "\n");
    fprintf(f, "  -h, -help                     Print this usage information.\n");
    fprintf(f, "\n");
    cut_usage(f);
    exit(exit_code);
}   /* usage() */

/* ------------------------------------------------------------------------- */
int main(int argc, char* argv[]) {
    int i = 0;
    g_program_name = argv[0];

    cut_parse_command_line(&argc, argv);

    CUT_INSTALL_SUITE(test_read_file);

    for (i = 1; i < argc; ++i) {
        if ((0 == strcmp(argv[i], "-h")) || (0 == strcmp(argv[i], "-help"))) {
            usage(stdout, 0);
        } else {
            if (!cut_include_test(argv[i])) {
                fprintf(stderr, "%s: no test names match '%s'\n", g_program_name, argv[i]);
                fprintf(stderr, "%s: use -h for usage information\n", g_program_name);
                exit(1);
            }
        }
    }

    return cut_run(1);
}   /* main() */
