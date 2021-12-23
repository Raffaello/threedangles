#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Mesh.hpp>

TEST(Mesh, plain_cube)
{
    auto m = Mesh::loadFromOBJFile("plain_cube.obj");

    ASSERT_NE(m, nullptr);
    
    EXPECT_EQ(m->tris.size(), 12);
    //EXPECT_EQ(m->vertexes.size(), 8);
    //EXPECT_EQ(m->faces_index.size(), 12);
    EXPECT_STRCASEEQ(m->name.c_str(), "PlainCube");
    EXPECT_TRUE(m->visible);

    EXPECT_EQ(m->adjacency_index.size(), 12);
    std::array<std::vector<unsigned short>, 3> exp = {
        {{1,7,10,11},{6,7,8,9},{1,2,9}},
    };

    //  Testing only the 1st face
    int i = 0;
    for (int j = 0; j < 3; j++) {
        //GTEST_LOG_(INFO) << "j=" << j;
        EXPECT_EQ(m->adjacency_index[i][j].size(), exp[j].size());
        for (int k = 0; k < m->adjacency_index[i][j].size(); k++) {
            EXPECT_EQ(m->adjacency_index[i][j][k], exp[j][k]);
        }
    }
}

TEST(Mesh, norml_cube)
{
    auto m = Mesh::loadFromOBJFile("normal_cube.obj");

    ASSERT_NE(m, nullptr);

    EXPECT_EQ(m->tris.size(), 12);
    EXPECT_STRCASEEQ(m->name.c_str(), "normal_cube.obj");
    EXPECT_TRUE(m->visible);
    EXPECT_EQ(m->adjacency_index.size(), 12);
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
