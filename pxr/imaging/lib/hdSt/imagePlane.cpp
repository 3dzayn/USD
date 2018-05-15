#include "imagePlane.h"

#include "pxr/imaging/hdSt/drawItem.h"
#include "pxr/imaging/hdSt/imagePlaneShaderKey.h"
#include "pxr/imaging/hdSt/meshTopology.h"
#include "pxr/imaging/hdSt/material.h"
#include "pxr/imaging/hdSt/package.h"

#include "pxr/imaging/hd/vtBufferSource.h"

#include "pxr/imaging/glf/contextCaps.h"

PXR_NAMESPACE_OPEN_SCOPE

HdStImagePlane::HdStImagePlane(const SdfPath& id, const SdfPath& instanceId)
    : HdImagePlane(id, instanceId), _topology(), _topologyId(0) {

}

void HdStImagePlane::Sync(
    HdSceneDelegate* delegate,
    HdRenderParam* /*renderParam*/,
    HdDirtyBits* dirtyBits,
    const TfToken& reprName,
    bool forcedRepr) {
    HdRprim::_Sync(delegate, reprName, forcedRepr, dirtyBits);

    auto calcReprName = _GetReprName(reprName, forcedRepr);
    _UpdateRepr(delegate, calcReprName, dirtyBits);

    *dirtyBits &= ~HdChangeTracker::AllSceneDirtyBits;
}

HdDirtyBits
HdStImagePlane::_GetInitialDirtyBits() const {
    HdDirtyBits mask = HdChangeTracker::Clean
                       | HdChangeTracker::InitRepr
                       | HdChangeTracker::DirtyPoints
                       | HdChangeTracker::DirtyTopology
                       | HdChangeTracker::DirtyTransform
                       | HdChangeTracker::DirtyPrimID
                       | HdChangeTracker::DirtyRepr
                       | HdChangeTracker::DirtyPrimvar
                       | HdChangeTracker::DirtyMaterialId
                       | HdChangeTracker::DirtyVisibility;

    return mask;
}

HdDirtyBits
HdStImagePlane::_PropagateDirtyBits(HdDirtyBits bits) const {
    return bits;
}

void
HdStImagePlane::_InitRepr(TfToken const &reprName, HdDirtyBits *dirtyBits) {
    const auto& descs = _GetReprDesc(reprName);

    if (_reprs.empty()) {
        _reprs.emplace_back(reprName, boost::make_shared<HdRepr>());

        auto& repr = _reprs.back().second;

        for (const auto& desc: descs) {
            if (desc.geomStyle == HdImagePlaneGeomStyleInvalid) { continue; }
            auto* drawItem = new HdStDrawItem(&_sharedData);
            repr->AddDrawItem(drawItem);
        }

        *dirtyBits |= HdChangeTracker::NewRepr;
    }
}

void
HdStImagePlane::_UpdateDrawItem(
    HdSceneDelegate* sceneDelegate,
    HdStDrawItem* drawItem,
    HdDirtyBits* dirtyBits) {

    _UpdateVisibility(sceneDelegate, dirtyBits);
    // TODO: replace this, since we have quite special needs.
    _PopulateConstantPrimvars(sceneDelegate, drawItem, dirtyBits);
    // TODO: we need to figure out what's required here
    // essentially we don't need any materials here, imagePlane.glslfx should
    // be able to handle everything.
    const auto materialId = GetMaterialId();
    // This was taken from a mesh.cpp utility function.
    TfToken mixinKey = sceneDelegate->GetShadingStyle(GetId()).GetWithDefault<TfToken>();
    static std::once_flag firstUse;
    static std::unique_ptr<GlfGLSLFX> mixinFX;

    std::call_once(firstUse, [](){
        std::string filePath = HdStPackageLightingIntegrationShader();
        mixinFX.reset(new GlfGLSLFX(filePath));
    });

    auto mixinSource = mixinFX->GetSource(mixinKey);

    drawItem->SetMaterialShaderFromRenderIndex(
        sceneDelegate->GetRenderIndex(), materialId, mixinSource);
    //drawItem->SetMaterialShaderFromRenderIndex(
    //    sceneDelegate->GetRenderIndex(), materialId);

    const auto& id = GetId();

    HdSt_ImagePlaneShaderKey shaderKey;
    HdStResourceRegistrySharedPtr resourceRegistry =
         boost::static_pointer_cast<HdStResourceRegistry>(
             sceneDelegate->GetRenderIndex().GetResourceRegistry());
    auto geometricShader = HdSt_GeometricShader::Create(shaderKey, resourceRegistry);

    drawItem->SetGeometricShader(geometricShader);

    auto& renderIndex = sceneDelegate->GetRenderIndex();
    renderIndex.GetChangeTracker().MarkShaderBindingsDirty();

    // TODO: We'll need points there and later on uvs
    // to control the texture mapping.
    if (HdChangeTracker::IsAnyPrimvarDirty(*dirtyBits, id)) {
        _PopulateVertexPrimvars(id, sceneDelegate, drawItem, dirtyBits);
    }

    if (*dirtyBits & HdChangeTracker::DirtyTopology) {
        _PopulateTopology(id, sceneDelegate, drawItem, dirtyBits);
    }

    // VertexPrimvar may be null, if there are no points in the prim.
    TF_VERIFY(drawItem->GetConstantPrimvarRange());
}

void
HdStImagePlane::_PopulateVertexPrimvars(
    const SdfPath& id,
    HdSceneDelegate* sceneDelegate,
    HdStDrawItem* drawItem,
    HdDirtyBits* dirtyBits) {

    const HdStResourceRegistrySharedPtr& resourceRegistry =
        boost::static_pointer_cast<HdStResourceRegistry>(
            sceneDelegate->GetRenderIndex().GetResourceRegistry());

    HdPrimvarDescriptorVector primvars =
        GetPrimvarDescriptors(sceneDelegate, HdInterpolationVertex);
    HdPrimvarDescriptorVector varyingPvs =
        GetPrimvarDescriptors(sceneDelegate, HdInterpolationVarying);
    primvars.insert(primvars.end(), varyingPvs.begin(), varyingPvs.end());

    HdBufferSourceVector sources;
    sources.reserve(primvars.size());

    size_t pointsIndexInSourceArray = std::numeric_limits<size_t>::max();

    for (const auto& primvar: primvars) {
        if (!HdChangeTracker::IsPrimvarDirty(*dirtyBits, id, primvar.name)) {
            continue;
        }

        auto value = GetPrimvar(sceneDelegate, primvar.name);

        if (!value.IsEmpty()) {
            if (primvar.name == HdTokens->points) {
                pointsIndexInSourceArray = sources.size();
            }

            HdBufferSourceSharedPtr source(new HdVtBufferSource(primvar.name, value));
            sources.push_back(source);
        }
    }

    if (sources.empty()) { return; }

    auto vertexPrimVarRange = drawItem->GetVertexPrimvarRange();
    if (!vertexPrimVarRange ||
        !vertexPrimVarRange->IsValid()) {
        HdBufferSpecVector bufferSpecs;
        // for (auto& it: sources) {
        //     it->AddBufferSpecs(&bufferSpecs);
        // }

        auto range = resourceRegistry->AllocateNonUniformBufferArrayRange(
                HdTokens->primvar, bufferSpecs);
        _sharedData.barContainer.Set(
            drawItem->GetDrawingCoord()->GetVertexPrimvarIndex(), range);
    } else if (pointsIndexInSourceArray != std::numeric_limits<size_t>::max()) {
        const auto previousRange = drawItem->GetVertexPrimvarRange()->GetNumElements();
        const auto newRange = sources[pointsIndexInSourceArray]->GetNumElements();

        if (previousRange != newRange) {
            sceneDelegate->GetRenderIndex().GetChangeTracker().SetGarbageCollectionNeeded();
        }
    }

    resourceRegistry->AddSources(
        drawItem->GetVertexPrimvarRange(), sources);
}

void
HdStImagePlane::_PopulateTopology(
    const SdfPath& id,
    HdSceneDelegate* sceneDelegate,
    HdStDrawItem* drawItem,
    HdDirtyBits* dirtyBits) {

    const HdStResourceRegistrySharedPtr& resourceRegistry =
        boost::static_pointer_cast<HdStResourceRegistry>(
            sceneDelegate->GetRenderIndex().GetResourceRegistry());

    if (HdChangeTracker::IsTopologyDirty(*dirtyBits, id)) {
        auto meshTopology = sceneDelegate->GetMeshTopology(id);
        HdSt_MeshTopologySharedPtr topology = HdSt_MeshTopology::New(meshTopology, 0);

        _topologyId = topology->ComputeHash();

        {
            HdInstance<HdTopology::ID, HdMeshTopologySharedPtr> topologyInstance;
            std::unique_lock<std::mutex> regLock =
                resourceRegistry->RegisterMeshTopology(_topologyId, &topologyInstance);

            if (topologyInstance.IsFirstInstance()) {
                topologyInstance.SetValue(
                    boost::static_pointer_cast<HdMeshTopology>(topology));
            }

            _topology = boost::static_pointer_cast<HdSt_MeshTopology>(topologyInstance.GetValue());
        }

        TF_VERIFY(_topology);
    }

    {
        HdInstance<HdTopology::ID, HdBufferArrayRangeSharedPtr> rangeInstance;

        std::unique_lock<std::mutex> regLock =
            resourceRegistry->RegisterMeshIndexRange(_topologyId, HdTokens->indices, &rangeInstance);

        if (rangeInstance.IsFirstInstance()) {
            HdBufferSourceSharedPtr source = _topology->GetTriangleIndexBuilderComputation(GetId());

            HdBufferSourceVector sources;
            sources.push_back(source);

            HdBufferSpecVector bufferSpecs;
            // HdBufferSpec::AddBufferSpecs(&bufferSpecs, sources);

            HdBufferArrayRangeSharedPtr range =
                resourceRegistry->AllocateNonUniformBufferArrayRange(
                    HdTokens->topology, bufferSpecs);

            resourceRegistry->AddSources(range, sources);
            rangeInstance.SetValue(range);
            if (drawItem->GetTopologyRange()) {
                sceneDelegate->GetRenderIndex().GetChangeTracker().SetGarbageCollectionNeeded();
            }
        }

        _sharedData.barContainer.Set(drawItem->GetDrawingCoord()->GetTopologyIndex(),
                                     rangeInstance.GetValue());
    }
}

void
HdStImagePlane::_UpdateRepr(
    HdSceneDelegate* sceneDelegate,
    const TfToken& reprName,
    HdDirtyBits* dirtyBits) {
    if (_reprs.empty()) {
        TF_CODING_ERROR("_InitRepr() should be called for repr %s.",
                        reprName.GetText());
        return;
    }
    if (HdChangeTracker::IsDirty(*dirtyBits)) {
        auto* drawItem = static_cast<HdStDrawItem*>(_reprs[0].second->GetDrawItem(0));
        _UpdateDrawItem(
            sceneDelegate,
            drawItem,
            dirtyBits);
        *dirtyBits &= ~HdChangeTracker::NewRepr;
    }
}

PXR_NAMESPACE_CLOSE_SCOPE

