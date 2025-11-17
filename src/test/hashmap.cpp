#include <gtest/gtest.h>
extern "C"
{
#include "../hashmap.h"
}

TEST(HashMap, Add)
{
    HashMap *map;
    void* ret;
    char* data = (char*)"data1";
    char* key = (char*)"key";

    map = hashmap_new();
    hashmap_add(map, data, key);

    ret = hashmap_get(map,  key);

    EXPECT_STREQ(data, (char*)ret);
}

TEST(HashMap, NoEntry)
{
    HashMap *map;
    char* key = (char*)"key";
    void* ret;
    map = hashmap_new();

    ret = hashmap_get(map, key);
    EXPECT_TRUE(ret == NULL);
}

TEST(HashMap, Overwrite)
{
    HashMap *map;
    void* ret;
    char* data1 = (char*)"data1";
    char* data2 = (char*)"data2";
    char* key = (char*)"key";

    map = hashmap_new();
    hashmap_add(map, data1, key);
    hashmap_add(map, data2, key);

    ret = hashmap_get(map,  key);

    EXPECT_STREQ(data2, (char*)ret);
}