#
# Copyright 2016 Pixar
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
#

#
# Edit the following to alter the set of custom attributes.
#
# Every entry should be an object derived from CustomAttribute,
# defined below.
#
def _GetCustomAttributes(currentPrim, rootDataModel):
    return ([BoundingBoxAttribute(currentPrim, rootDataModel),
               LocalToWorldXformAttribute(currentPrim, rootDataModel)],
            [RelationshipAttribute(currentPrim, relationship) \
                    for relationship in currentPrim.GetRelationships()])

#
# The base class for per-prim custom attributes.
#
class CustomAttribute:
    def __init__(self, currentPrim):
        self._currentPrim = currentPrim

    def IsVisible(self):
        return True

    # GetName function to match UsdAttribute API
    def GetName(self):
        return ""

    # Get function to match UsdAttribute API
    def Get(self, frame):
        return ""

    # convenience function to make this look more like a UsdAttribute
    def GetTypeName(self):
        return ""

#
# Displays the bounding box of a prim
#
class BoundingBoxAttribute(CustomAttribute):
    def __init__(self, currentPrim, rootDataModel):
        CustomAttribute.__init__(self, currentPrim)
        self.rootDataModel = rootDataModel

    def GetName(self):
        return "World Bounding Box"

    def Get(self, frame):
        try:
            bbox = self.rootDataModel.computeWorldBound(self._currentPrim)

        except RuntimeError, err:
            bbox = "Invalid: " + str(err)

        return bbox

#
# Displays the Local to world xform of a prim
#
class LocalToWorldXformAttribute(CustomAttribute):
    def __init__(self, currentPrim, rootDataModel):
        CustomAttribute.__init__(self, currentPrim)
        self.rootDataModel = rootDataModel

    def GetName(self):
        return "Local to World Xform"

    def Get(self, frame):
        try:
            pwt = self.rootDataModel.getLocalToWorldTransform(self._currentPrim)
        except RuntimeError, err:
            pwt = "Invalid: " + str(err)

        return pwt

#
# Displays a relationship on the prim
#
class RelationshipAttribute(CustomAttribute):
    def __init__(self, currentPrim, relationship):
        CustomAttribute.__init__(self, currentPrim)
        self._relationship = relationship

    def GetName(self):
        return self._relationship.GetName()

    def Get(self, frame):
        return self._relationship.GetTargets()
