//=============================================================================
// Copyright (C) 2017-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include "gtest/gtest.h"

#include <pmp/algorithms/SurfaceGeodesic.h>

using namespace pmp;

TEST(SurfaceGeodesicTest, geodesic)
{
    // read mesh for unit sphere
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/sphere.off"));

    // use first vertex as seed
    std::vector<Vertex> seed;
    seed.push_back(Vertex(0));

    // compute geodesic distance
    SurfaceGeodesic geodist(mesh, seed);

    // find maximum geodesic distance
    Scalar d(0);
    for (auto v: mesh.vertices())
        d = std::max(d, geodist(v));
    EXPECT_FLOAT_EQ(d, 3.1348989);

    // map distances to texture coordinates
    geodist.distance_to_texture_coordinates();
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    EXPECT_TRUE(tex);
}

TEST(SurfaceGeodesicTest, geodesic_symmetry)
{
    // read irregular mesh (to have virtual edges)
    SurfaceMesh mesh;
    EXPECT_TRUE(mesh.read("pmp-data/off/bunny_adaptive.off"));

    SurfaceGeodesic geodist(mesh);
    std::vector<Vertex> seed;
    Vertex v0, v1;
    Scalar d0, d1;

    // grow from first vector
    v0 = Vertex(0);
    seed.clear();
    seed.push_back(v0);
    geodist.compute(seed);

    // find maximum geodesic distance
    d0=0;
    for (auto v: mesh.vertices())
    {
        if (geodist(v) > d0)
        {
            d0 = geodist(v);
            v1 = v;
        }
    }

    // grow back from max-dist vertex to vertex 0
    seed.clear();
    seed.push_back(v1);
    geodist.compute(seed);
    d1 = geodist(v0);

    // expect both distance to be the same
    Scalar err = fabs(d0-d1) / (0.5*(d0+d1));
    EXPECT_LT(err, 0.001);
}
