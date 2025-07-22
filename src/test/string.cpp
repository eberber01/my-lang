#include <gtest/gtest.h>
extern "C"
{
#include "../util.h"
}

TEST(String, New)
{
    String *s = string_new();
    EXPECT_EQ(s->length, 0);
    string_free(s);
}

TEST(String, AsStr)
{
    String *s;
    char *str;
    char target[6] = "hello";
    s = string_new();

    str = as_str(s);
    free(str);
    EXPECT_STREQ("", "");

    for (int i = 0; i < 6; i++)
    {
        string_append(s, target[i]);
    }
    str = as_str(s);
    EXPECT_STREQ(str, target);
    string_free(s);
    free(str);
}
