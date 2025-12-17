
#include <gtest/gtest.h>
extern "C"
{
#include <mylang/util.h>
}

struct VectorData
{
};

TEST(Vector, Get)
{
    VectorData *ptrs[3];
    VectorData *data;
    Vector *v = vector_new();
    for (int i = 0; i < 3; i++)
    {
        ptrs[i] = (VectorData *)my_malloc(sizeof(VectorData));
        vector_push(v, ptrs[i]);
    }
    EXPECT_EQ(3, v->length);

    for (int i = 0; i < 3; i++)
    {
        data = (VectorData *)vector_get(v, i);
        EXPECT_EQ(ptrs[i], data);
    }

    vector_free(v);
}

TEST(Vector, Push)
{
    VectorData *data;
    Vector *v = vector_new();
    data = (VectorData *)my_malloc(sizeof(VectorData));
    vector_push(v, data);
    EXPECT_EQ(data, v->array[0]);

    vector_free(v);
}

TEST(Vector, New)
{
    VectorData *data;
    Vector *v = vector_new();

    EXPECT_EQ(VECTOR_INIT_SIZE, v->size);
    EXPECT_EQ(v->length, 0);
    vector_free(v);
}

TEST(Vector, Resize)
{
    VectorData *data;
    Vector *v = vector_new();
    for (int i = 0; i < VECTOR_INIT_SIZE + 1; i++)
    {
        data = (VectorData *)my_malloc(sizeof(VectorData));
        vector_push(v, data);
    }
    EXPECT_EQ(VECTOR_INIT_SIZE * 2, v->size);
    vector_free(v);
}