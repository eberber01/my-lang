#include <gtest/gtest.h>
extern "C"
{
#include <mylang/util.h>
}

TEST(String, New)
{
    String *s = string_new();
    EXPECT_EQ(s->length, 0);
    arena_free(context_arena);
}

TEST(String, AsStr)
{
    String *s;
    char *str;
    char target[6] = "hello";
    s = string_new();

    str = as_str(s);
    EXPECT_STREQ("", "");

    for (int i = 0; i < 6; i++)
    {
        string_append(s, target[i]);
    }
    str = as_str(s);
    EXPECT_STREQ(str, target);
    arena_free(context_arena);
}
