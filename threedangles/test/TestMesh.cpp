#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Mesh.hpp>

TEST(Mesh, loadOBJ_plain_cube)
{
    auto m = Mesh::loadFromOBJFile("plain_cube.obj");

    ASSERT_NE(m, nullptr);
    
    EXPECT_EQ(m->tris.size(), 12);
    //EXPECT_EQ(m->vertexes.size(), 8);
    //EXPECT_EQ(m->faces_index.size(), 12);
    EXPECT_STRCASEEQ(m->name.c_str(), "PlainCube");
    EXPECT_TRUE(m->visible);
    //EXPECT_EQ(m->surface_normals.size(), 12);

    //EXPECT_EQ(m->adjacency_index.size(), 12);
}

TEST(Mesh, plain_cube_adj_index)
{
    auto m = Mesh::loadFromOBJFile("plain_cube.obj");
    ASSERT_NE(m, nullptr);

    /*EXPECT_EQ(m->adjacency_index.size(), 12);
    EXPECT_EQ(m->adjacency_index[0][0].size(), 1);
    EXPECT_EQ(m->adjacency_index[0][1].size(), 1);
    EXPECT_EQ(m->adjacency_index[0][2].size(), 1);*/
    /*EXPECT_EQ(m->adjacency_index[1][0].size(), 4);
    EXPECT_EQ(m->adjacency_index[1][1].size(), 3);
    EXPECT_EQ(m->adjacency_index[1][2].size(), 3);
    EXPECT_EQ(m->adjacency_index[2][0].size(), 4);
    EXPECT_EQ(m->adjacency_index[2][1].size(), 4);
    EXPECT_EQ(m->adjacency_index[2][2].size(), 3);
    EXPECT_EQ(m->adjacency_index[3][0].size(), 3);
    EXPECT_EQ(m->adjacency_index[3][1].size(), 0);
    EXPECT_EQ(m->adjacency_index[3][2].size(), 0);
    EXPECT_EQ(m->adjacency_index[10].size(), 0);
    EXPECT_EQ(m->adjacency_index[11].size(), 0);*/
}

TEST(Mesh, norml_cube_load)
{
    auto m = Mesh::loadFromOBJFile("normal_cube.obj");

    ASSERT_NE(m, nullptr);

    EXPECT_EQ(m->tris.size(), 12);
    //EXPECT_EQ(m->vertexes.size(), 8);
    //EXPECT_EQ(m->faces_index.size(), 12);
    EXPECT_STRCASEEQ(m->name.c_str(), "normal_cube.obj");
    EXPECT_TRUE(m->visible);
    //EXPECT_EQ(m->surface_normals.size(), 12);

    //EXPECT_EQ(m->adjacency_index.size(), 12);

}



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
