#!/pxrpythonsubst
#
# Copyright 2017 Pixar
#
# Licensed under the Apache License, Version 2.0 (the "Apache License")
# with the following modification; you may not use this file except in
# compliance with the Apache License and the following modification to it:
# Section 6. Trademarks. is deleted and replaced with:
#
# 6. Trademarks. This License does not grant permission to use the trade
#    names, trademarks, service marks, or product names of the Licensor
#    and its affiliates, except as required to comply with Section 4(c) of
#    the License and to reproduce the content of the NOTICE file.
#
# You may obtain a copy of the Apache License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the Apache License with the above modification is
# distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the Apache License for the specific
# language governing permissions and limitations under the Apache License.

from pxr import Usd, UsdSkel, UsdGeom, Vt, Sdf
import unittest


class TestUsdSkelCache(unittest.TestCase):


    def test_AnimQuery(self):
        """Tests anim query retrieval."""

        cache = UsdSkel.Cache()

        stage = Usd.Stage.CreateInMemory()
         
        anim = UsdSkel.PackedJointAnimation.Define(stage, "/Anim")
        
        assert cache.GetAnimQuery(anim.GetPrim())

        assert not cache.GetAnimQuery(stage.DefinePrim("/Foo"))
        
        assert not cache.GetAnimQuery(
            UsdGeom.Xform.Define(stage, "/Bar").GetPrim())

        # TODO: Test query of anim behind instancing


    def test_Populate(self):
        """Tests for correctness in interpretation of inherited bindings,
           as computed through Populate()."""

        testFile = "populate.usda"
        stage = Usd.Stage.Open(testFile)

        rootPath = "/InheritedBindings"

        cache = UsdSkel.Cache()
        root = UsdSkel.Root(stage.GetPrimAtPath(rootPath))
        assert cache.Populate(root)

        def _GetSkelQuery(path):
            return cache.GetSkelQuery(stage.GetPrimAtPath(path))

        def _GetInheritedSkelQuery(path):
            return cache.GetInheritedSkelQuery(stage.GetPrimAtPath(path))

        def _GetSkelPath(skelQuery):
            return skelQuery.GetSkeleton().GetPrim().GetPath()

        def _GetAnimPath(skelQuery):
            return skelQuery.GetAnimQuery().GetPrim().GetPath()

        skel = _GetSkelQuery(rootPath+"/Model1")
        self.assertEqual(_GetSkelPath(skel), Sdf.Path("/Skel1"))
        assert not skel.GetAnimQuery()

        # an animationSource does not itself define a skel binding.
        # bindings are associated with skel:skeleton rels only!
        skel = _GetSkelQuery(rootPath+"/Model1/A")
        assert not skel

        skel = _GetSkelQuery(rootPath+"/Model1/A/B")
        self.assertEqual(_GetSkelPath(skel), Sdf.Path("/Skel2"))
        self.assertEqual(_GetAnimPath(skel), Sdf.Path("/Anim1"))

        skel = _GetSkelQuery(rootPath+"/Model1/C")
        self.assertEqual(_GetSkelPath(skel), Sdf.Path("/Skel3"))
        self.assertEqual(_GetAnimPath(skel), Sdf.Path("/Anim2"))

        # Inherited skel queries?
        self.assertEqual(
            _GetSkelPath(_GetInheritedSkelQuery(rootPath+"/Model1/A/B/Gprim")),
            _GetSkelPath(_GetSkelQuery(rootPath+"/Model1/A/B")))

        self.assertEqual(
            _GetSkelPath(_GetInheritedSkelQuery(rootPath+"/Model1/A/B")),
            _GetSkelPath(_GetSkelQuery(rootPath+"/Model1/A/B")))

        # Scope with no bound skel.
        assert not _GetSkelQuery(rootPath+"/Model2")
        assert not cache.GetSkelQuery(Usd.Prim())

        # Scope with a bound skel, but whose animation source is inactive.
        skel = _GetSkelQuery(rootPath+"/Model3/SkelWithInactiveAnim")
        self.assertEqual(_GetSkelPath(skel), Sdf.Path("/Skel1"))
        assert not skel.GetAnimQuery()

        def _GetSkinningQuery(path):
            return cache.GetSkinningQuery(stage.GetPrimAtPath(path))

        # Make sure some scopes are not being treated as skinning targets.
        nonSkinnablePaths = (rootPath+"/Model1/NonRigidScope1",
                             rootPath+"/Model1/NonRigidScope2",
                             rootPath+"/Model1/RigidScopeParent")
                          
        for path in nonSkinnablePaths:
            assert not _GetSkinningQuery(path)

        query = _GetSkinningQuery(rootPath+"/Model1/NonRigidScope1/A")
        assert query
        assert query.IsRigidlyDeformed()
        assert query.GetMapper()
        self.assertEquals(query.GetJointOrder(),
                          Vt.TokenArray(["A", "B", "C"]))

        query = _GetSkinningQuery(rootPath+"/Model1/NonRigidScope1/B")
        assert query
        assert not query.IsRigidlyDeformed()
        assert not query.GetMapper()

        query = _GetSkinningQuery(rootPath+"/Model1/NonRigidScope2/A")
        assert query
        assert query.IsRigidlyDeformed()
        assert query.GetMapper()
        self.assertEquals(query.GetJointOrder(),
                          Vt.TokenArray(["A", "B", "C"]))

        query = _GetSkinningQuery(rootPath+"/Model1/NonRigidScope2/B")
        assert query
        assert query.IsRigidlyDeformed()
        assert not query.GetMapper()

        # TODO: When adding support for rigid deformation of intermediate
        # xformables, Model1/RigidScopeParent/RigidScope should be treated
        # as being skinnable.
        assert _GetSkinningQuery(rootPath+"/Model1/RigidScopeParent/RigidScope/A")
        assert _GetSkinningQuery(rootPath+"/Model1/RigidScopeParent/RigidScope/B")


if __name__ == "__main__":
    unittest.main()
