
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2012 Francois Beaune, Jupiter Jazz Limited
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// appleseed.renderer headers.
#include "renderer/global/globaltypes.h"
#include "renderer/modeling/object/object.h"
#include "renderer/modeling/scene/assembly.h"
#include "renderer/modeling/scene/assemblyinstance.h"
#include "renderer/modeling/scene/containers.h"
#include "renderer/modeling/scene/objectinstance.h"
#include "renderer/utility/paramarray.h"
#include "renderer/utility/testutils.h"

// appleseed.foundation headers.
#include "foundation/math/matrix.h"
#include "foundation/math/transform.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/test.h"

using namespace foundation;
using namespace renderer;

TEST_SUITE(Renderer_Modeling_Scene_AssemblyInstance)
{
    TEST_CASE(ComputeLocalBBox_TakesChildAssemblyInstanceIntoAccount)
    {
        // Inner assembly.

        auto_release_ptr<Assembly> inner_assembly =
            AssemblyFactory::create("inner_assembly", ParamArray());

        inner_assembly->objects().insert(
            auto_release_ptr<Object>(
                new BoundingBoxObject(
                    "object",
                    GAABB3(GVector3(-1.0), GVector3(1.0)))));

        inner_assembly->object_instances().insert(
            ObjectInstanceFactory::create(
                "object_instance",
                ParamArray(),
                "object",
                Transformd::identity(),
                StringArray()));

        auto_release_ptr<AssemblyInstance> inner_assembly_instance =
            AssemblyInstanceFactory::create(
                "inner_assembly_instance",
                ParamArray(),
                inner_assembly.ref());

        inner_assembly_instance->transform_sequence().set_transform(
            0.0,
            Transformd(Matrix4d::scaling(Vector3d(10.0))));

        // Outer assembly.

        auto_release_ptr<Assembly> outer_assembly =
            AssemblyFactory::create(
                "outer_assembly",
                ParamArray());

        outer_assembly->assemblies().insert(inner_assembly);
        outer_assembly->assembly_instances().insert(inner_assembly_instance);

        auto_release_ptr<AssemblyInstance> outer_assembly_instance =
            AssemblyInstanceFactory::create(
                "outer_assembly_instance",
                ParamArray(),
                outer_assembly.ref());

        const GAABB3 local_bbox = outer_assembly_instance->compute_local_bbox();

        EXPECT_EQ(GAABB3(GVector3(-10.0), GVector3(10.0)), local_bbox);
    }
}
