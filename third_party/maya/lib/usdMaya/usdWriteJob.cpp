//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "pxr/pxr.h"
#include "usdMaya/usdWriteJob.h"

#include "usdMaya/JobArgs.h"
#include "usdMaya/translatorLook.h"
#include "usdMaya/PluginPrimWriter.h"

#include "usdMaya/Chaser.h"
#include "usdMaya/ChaserRegistry.h"

#include "pxr/usd/ar/resolver.h"
#include "pxr/usd/usd/modelAPI.h"
#include "pxr/usd/kind/registry.h"

#include "pxr/usd/usd/variantSets.h"
#include "pxr/usd/usd/editContext.h"
#include "pxr/usd/usd/treeIterator.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/usdUtils/pipeline.h"
#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/hashset.h"
#include "pxr/base/tf/stl.h"

#include "pxr/usd/sdf/layer.h"
#include "pxr/usd/sdf/primSpec.h"
// Needed for directly removing a UsdVariant via Sdf
//   Remove when UsdVariantSet::RemoveVariant() is exposed
//   XXX [bug 75864]
#include "pxr/usd/sdf/variantSetSpec.h"
#include "pxr/usd/sdf/variantSpec.h"

#include <maya/MFnDagNode.h>
#include <maya/MFnRenderLayer.h>
#include <maya/MItDag.h>
#include <maya/MObjectArray.h>
#include <maya/MPxNode.h>
#include <maya/MDagPathArray.h>

#include <limits>
#include <map>
#include <unordered_set>

PXR_NAMESPACE_OPEN_SCOPE


usdWriteJob::usdWriteJob(const JobExportArgs & iArgs) :
    usdWriteJobCtx(iArgs), mModelKindWriter(iArgs)
{
}


usdWriteJob::~usdWriteJob()
{
}

bool usdWriteJob::beginJob(bool append)
{
    // Check for DAG nodes that are a child of an already specified DAG node to export
    // if that's the case, report the issue and skip the export
    PxrUsdMayaUtil::ShapeSet::const_iterator m, n;
    PxrUsdMayaUtil::ShapeSet::const_iterator endPath = mArgs.dagPaths.end();
    for (m = mArgs.dagPaths.begin(); m != endPath; ) {
        MDagPath path1 = *m; m++;
        for (n = m; n != endPath; n++) {
            MDagPath path2 = *n;
            if (PxrUsdMayaUtil::isAncestorDescendentRelationship(path1,path2)) {
                MString errorMsg = path1.fullPathName();
                errorMsg += " and ";
                errorMsg += path2.fullPathName();
                errorMsg += " have an ancestor relationship. Skipping USD Export.";
                MGlobal::displayError(errorMsg);
                return false;
            }
        }  // for n
    }  // for m

    // Make sure the file name is a valid one with a proper USD extension.
    const std::string iFileExtension = TfStringGetSuffix(mArgs.fileName, '.');
    if (!UsdStage::IsSupportedFile(mArgs.fileName)) {
        mArgs.fileName = TfStringPrintf("%s.%s",
                                        TfStringGetBeforeSuffix(mArgs.fileName, '.').c_str(),
                                        PxrUsdMayaTranslatorTokens->UsdFileExtensionDefault.GetText());
    }

    MGlobal::displayInfo("usdWriteJob::beginJob: Create stage file "+MString(mArgs.fileName.c_str()));

    if (!openFile(mArgs.fileName, append)) {
        return false;
    }

    // Set time range for the USD file
    mStage->SetStartTimeCode(mArgs.startTime);
    mStage->SetEndTimeCode(mArgs.endTime);
    
    mModelKindWriter.Reset();

    // Setup the requested render layer mode:
    //     defaultLayer    - Switch to the default render layer before exporting,
    //                       then switch back afterwards (no layer switching if
    //                       the current layer IS the default layer).
    //     currentLayer    - No layer switching before or after exporting. Just
    //                       use whatever is the current render layer for export.
    //     modelingVariant - Switch to the default render layer before exporting,
    //                       and export each render layer in the scene as a
    //                       modeling variant, then switch back afterwards (no
    //                       layer switching if the current layer IS the default
    //                       layer). The default layer will be made the default
    //                       modeling variant.
    MFnRenderLayer currentLayer(MFnRenderLayer::currentLayer());
    mCurrentRenderLayerName = currentLayer.name();

    if (mArgs.renderLayerMode == PxUsdExportJobArgsTokens->modelingVariant) {
        // Handle usdModelRootOverridePath for USD Variants
        MFnRenderLayer::listAllRenderLayers(mRenderLayerObjs);
        if (mRenderLayerObjs.length() > 1) {
            mArgs.usdModelRootOverridePath = SdfPath("/_BaseModel_");
        }
    }

    // Switch to the default render layer unless the renderLayerMode is
    // 'currentLayer', or the default layer is already the current layer.
    if (mArgs.renderLayerMode != PxUsdExportJobArgsTokens->currentLayer &&
            MFnRenderLayer::currentLayer() != MFnRenderLayer::defaultRenderLayer()) {
        // Set the RenderLayer to the default render layer
        MFnRenderLayer defaultLayer(MFnRenderLayer::defaultRenderLayer());
        MGlobal::executeCommand(MString("editRenderLayerGlobals -currentRenderLayer ")+
                                        defaultLayer.name(), false, false);
    }

    // Pre-process the argument dagPath path names into two sets. One set
    // contains just the arg dagPaths, and the other contains all parents of
    // arg dagPaths all the way up to the world root. Partial path names are
    // enough because Maya guarantees them to still be unique, and they require
    // less work to hash and compare than full path names.
    TfHashSet<std::string, TfHash> argDagPaths;
    TfHashSet<std::string, TfHash> argDagPathParents;
    PxrUsdMayaUtil::ShapeSet::const_iterator end = mArgs.dagPaths.end();
    for (PxrUsdMayaUtil::ShapeSet::const_iterator it = mArgs.dagPaths.begin();
            it != end; ++it) {
        MDagPath curDagPath = *it;
        std::string curDagPathStr(curDagPath.partialPathName().asChar());
        argDagPaths.insert(curDagPathStr);

        while (curDagPath.pop() && curDagPath.length() >= 0) {
            curDagPathStr = curDagPath.partialPathName().asChar();
            if (argDagPathParents.find(curDagPathStr) != argDagPathParents.end()) {
                // We've already traversed up from this path.
                break;
            }
            argDagPathParents.insert(curDagPathStr);
        }
    }

    // Now do a depth-first traversal of the Maya DAG from the world root.
    // We keep a reference to arg dagPaths as we encounter them.
    MDagPath curLeafDagPath;
    MItDag itDag(MItDag::kDepthFirst, MFn::kInvalid);
    itDag.traverseUnderWorld(true);

    if (!mArgs.exportRootPath.empty()){
        // If a root is specified, start iteration there
        MDagPath rootDagPath;
        PxrUsdMayaUtil::GetDagPathByName(mArgs.exportRootPath, rootDagPath);
        itDag.reset(rootDagPath, MItDag::kDepthFirst, MFn::kInvalid);
    }

    for (; !itDag.isDone(); itDag.next()) {
        MDagPath curDagPath;
        itDag.getPath(curDagPath);
        std::string curDagPathStr(curDagPath.partialPathName().asChar());

        if (argDagPathParents.find(curDagPathStr) != argDagPathParents.end()) {
            // This dagPath is a parent of one of the arg dagPaths. It should
            // be included in the export, but not necessarily all of its
            // children should be, so we continue to traverse down.
        } else if (argDagPaths.find(curDagPathStr) != argDagPaths.end()) {
            // This dagPath IS one of the arg dagPaths. It AND all of its
            // children should be included in the export.
            curLeafDagPath = curDagPath;
        } else {
            MFnDagNode dagNode(curDagPath);
            auto hasParent = false;
            if (dagNode.inUnderWorld()) {
                for (auto dagPathCopy = curDagPath; dagPathCopy.pathCount(); dagPathCopy.pop()) {
                    MFnDagNode dagNodeCopy(dagPathCopy);
                    if (!dagNodeCopy.inUnderWorld()) {
                        hasParent = dagNodeCopy.hasParent(curLeafDagPath.node());
                        break;
                    }
                }
            } else {
                hasParent = dagNode.hasParent(curLeafDagPath.node());
            }
            if (!hasParent) {
                itDag.prune();
                continue;
            }
        }

        if (!needToTraverse(curDagPath) &&
            curDagPath.length() > 0) {
            // This dagPath and all of its children should be pruned.
            itDag.prune();
        } else {
            MayaPrimWriterPtr primWriter = createPrimWriter(curDagPath);

            if (primWriter) {
                mMayaPrimWriterList.push_back(primWriter);

                primWriter->write(UsdTimeCode::Default());

                // Write out data (non-animated/default values).
                if (const auto& usdPrim = primWriter->getPrim()) {
                    MDagPath dag = primWriter->getDagPath();
                    mDagPathToUsdPathMap[dag] = usdPrim.GetPath();

                    // If we are merging transforms and the object derives from
                    // MayaTransformWriter but isn't actually a transform node, we
                    // need to add its parent.
                    if (mArgs.mergeTransformAndShape) {
                        MayaTransformWriterPtr xformWriter =
                            std::dynamic_pointer_cast<MayaTransformWriter>(primWriter);
                        if (xformWriter) {
                            MDagPath xformDag = xformWriter->getTransformDagPath();
                            mDagPathToUsdPathMap[xformDag] = usdPrim.GetPath();
                        }
                    }
                }

                if (primWriter->shouldPruneChildren()) {
                    itDag.prune();
                }
            }
        }
    }

    // Writing Looks/Shading
    PxrUsdMayaTranslatorLook::ExportShadingEngines(
                mStage, 
                mArgs.dagPaths,
                mArgs.shadingMode,
                mArgs.mergeTransformAndShape,
                mArgs.handleUsdNamespaces,
                mArgs.usdModelRootOverridePath);

    if (!mModelKindWriter.MakeModelHierarchy(mStage)) {
        return false;
    }

    // now we populate the chasers and run export default
    mChasers.clear();
    PxrUsdMayaChaserRegistry::FactoryContext ctx(mStage, mDagPathToUsdPathMap, mArgs);
    for (const std::string& chaserName : mArgs.chaserNames) {
        if (PxrUsdMayaChaserRefPtr fn = 
                PxrUsdMayaChaserRegistry::GetInstance().Create(chaserName, ctx)) {
            mChasers.push_back(fn);
        }
        else {
            std::string error = TfStringPrintf("Failed to create chaser: %s",
                                               chaserName.c_str());
            MGlobal::displayError(MString(error.c_str()));
        }
    }

    for (const PxrUsdMayaChaserRefPtr& chaser : mChasers) {
        if (!chaser->ExportDefault()) {
            return false;
        }
    }

    return true;
}

//
// Inputs: 
//  Frame to process
void usdWriteJob::evalJob(double iFrame)
{
    for ( MayaPrimWriterPtr const & primWriter :  mMayaPrimWriterList) {
        UsdTimeCode usdTime(iFrame);
        primWriter->write(usdTime);
    }
    for (PxrUsdMayaChaserRefPtr& chaser: mChasers) {
        chaser->ExportFrame(iFrame);
    }
    perFrameCallback(iFrame);
}


void usdWriteJob::endJob()
{
    UsdPrimSiblingRange usdRootPrims = mStage->GetPseudoRoot().GetChildren();
    
    // Write Variants (to first root prim path)
    UsdPrim usdRootPrim;
    TfToken defaultPrim;

    if (!usdRootPrims.empty()) {
        usdRootPrim = *usdRootPrims.begin();
        defaultPrim = usdRootPrim.GetName();
    }

    if (usdRootPrim && mRenderLayerObjs.length() > 1 && 
        !mArgs.usdModelRootOverridePath.IsEmpty()) {
            // Get RenderLayers
            //   mArgs.usdModelRootOverridePath:
            //     Require mArgs.usdModelRootOverridePath to be set so that 
            //     the variants are put under a UsdPrim that references a BaseModel
            //     prim that has all of the geometry, transforms, and other details.
            //     This needs to be done since "local" values have stronger precedence
            //     than "variant" values, but "referencing" will cause the variant values
            //     to take precedence.
        defaultPrim = writeVariants(usdRootPrim);
    }

    // Restoring the currentRenderLayer
    MFnRenderLayer currentLayer(MFnRenderLayer::currentLayer());
    if (currentLayer.name() != mCurrentRenderLayerName) {
        MGlobal::executeCommand(MString("editRenderLayerGlobals -currentRenderLayer ")+
                                        mCurrentRenderLayerName, false, false);
    }

    postCallback();
    // clear this so that no stage references are left around
    // also, we are triggering a before save cleanup here
    mMayaPrimWriterList.clear();
    // Unfortunately, MGlobal::isZAxisUp() is merely session state that does
    // not get recorded in Maya files, so we cannot rely on it being set
    // properly.  Since "Y" is the more common upAxis, we'll just use
    // isZAxisUp as an override to whatever our pipeline is configured for.
    TfToken upAxis = UsdGeomGetFallbackUpAxis();
    if (MGlobal::isZAxisUp()){
        upAxis = UsdGeomTokens->z;
    }
    UsdGeomSetStageUpAxis(mStage, upAxis);
    if (usdRootPrim){
        // We have already decided above that 'usdRootPrim' is the important
        // prim for the export... usdVariantRootPrimPath
        mStage->GetRootLayer()->SetDefaultPrim(defaultPrim);
    }
    saveAndCloseStage();
    mMayaPrimWriterList.clear(); // clear this so that no stage references are left around
    MGlobal::displayInfo("usdWriteJob::endJob Saving Stage");
}

TfToken usdWriteJob::writeVariants(const UsdPrim &usdRootPrim)
{
    // Init parameters for filtering and setting the active variant
    std::string defaultModelingVariant;

    // Get the usdVariantRootPrimPath (optionally filter by renderLayer prefix)
    MayaPrimWriterPtr firstPrimWriterPtr = *mMayaPrimWriterList.begin();
    std::string firstPrimWriterPathStr(PxrUsdMayaUtil::MDagPathToUsdPathString(
        firstPrimWriterPtr->getDagPath(), mArgs.handleUsdNamespaces));
    SdfPath usdVariantRootPrimPath(firstPrimWriterPathStr);
    usdVariantRootPrimPath = usdVariantRootPrimPath.GetPrefixes()[0];

    // Create a new usdVariantRootPrim and reference the Base Model UsdRootPrim
    //   This is done for reasons as described above under mArgs.usdModelRootOverridePath
    UsdPrim usdVariantRootPrim = mStage->DefinePrim(usdVariantRootPrimPath);
    TfToken defaultPrim = usdVariantRootPrim.GetName();
    usdVariantRootPrim.GetReferences().AppendInternalReference(usdRootPrim.GetPath());
    usdVariantRootPrim.SetActive(true);
    usdRootPrim.SetActive(false);

    // Loop over all the renderLayers
    for (unsigned int ir=0; ir < mRenderLayerObjs.length(); ++ir) {
        SdfPathTable<bool> tableOfActivePaths;
        MFnRenderLayer renderLayerFn( mRenderLayerObjs[ir] );
        MString renderLayerName = renderLayerFn.name();
        std::string variantName(renderLayerName.asChar());
        // Determine default variant. Currently unsupported
        //MPlug renderLayerDisplayOrderPlug = renderLayerFn.findPlug("displayOrder", true);
        //int renderLayerDisplayOrder = renderLayerDisplayOrderPlug.asShort();
                    
        // The Maya default RenderLayer is also the default modeling variant
        if (mRenderLayerObjs[ir] == MFnRenderLayer::defaultRenderLayer()) {
            defaultModelingVariant=variantName;
        }
        
        // Make the renderlayer being looped the current one
        MGlobal::executeCommand(MString("editRenderLayerGlobals -currentRenderLayer ")+
                                        renderLayerName, false, false);

        // == ModelingVariants ==
        // Identify prims to activate
        // Put prims and parent prims in a SdfPathTable
        // Then use that membership to determine if a prim should be Active.
        // It has to be done this way since SetActive(false) disables access to all child prims.
        MObjectArray renderLayerMemberObjs;
        renderLayerFn.listMembers(renderLayerMemberObjs);
        std::vector< SdfPath > activePaths;
        for (unsigned int im=0; im < renderLayerMemberObjs.length(); ++im) {
            MFnDagNode dagFn(renderLayerMemberObjs[im]);
            MDagPath dagPath;
            dagFn.getPath(dagPath);
            dagPath.extendToShape();
            SdfPath usdPrimPath; 
            if (!TfMapLookup(mDagPathToUsdPathMap, dagPath, &usdPrimPath)) {
                continue;
            }
            usdPrimPath = usdPrimPath.ReplacePrefix(usdPrimPath.GetPrefixes()[0], usdVariantRootPrimPath); // Convert base to variant usdPrimPath
            tableOfActivePaths[usdPrimPath] = true;
            activePaths.push_back(usdPrimPath);
            //UsdPrim usdPrim = mStage->GetPrimAtPath(usdPrimPath);
            //usdPrim.SetActive(true);
        }
        if (!tableOfActivePaths.empty()) {
            { // == BEG: Scope for Variant EditContext
                // Create the variantSet and variant
                UsdVariantSet modelingVariantSet = usdVariantRootPrim.GetVariantSets().AppendVariantSet("modelingVariant");
                modelingVariantSet.AppendVariant(variantName);
                modelingVariantSet.SetVariantSelection(variantName);
                // Set the Edit Context
                UsdEditTarget editTarget = modelingVariantSet.GetVariantEditTarget();
                UsdEditContext editContext(mStage, editTarget);

                // == Activate/Deactivate UsdPrims
                UsdTreeIterator it = UsdTreeIterator::AllPrims(mStage->GetPseudoRoot());
                std::vector<UsdPrim> primsToDeactivate;
                for ( ; it; ++it) {
                    UsdPrim usdPrim = *it;
                    // For all xformable usdPrims...
                    if (usdPrim && usdPrim.IsA<UsdGeomXformable>()) {
                        bool isActive=false;
                        for (size_t j=0;j<activePaths.size();j++) {
                            //primPathD.HasPrefix(primPathA);
                            SdfPath activePath=activePaths[j];
                            if (usdPrim.GetPath().HasPrefix(activePath) || activePath.HasPrefix(usdPrim.GetPath())) {
                                isActive=true; break;
                            }
                        }
                        if (isActive==false) {
                            primsToDeactivate.push_back(usdPrim);
                            it.PruneChildren();
                        }
                    }
                }
                // Now deactivate the prims (done outside of the UsdTreeIterator 
                // so not to modify the iterator while in the loop)
                for ( UsdPrim const& prim : primsToDeactivate ) {
                    prim.SetActive(false);
                }
            } // == END: Scope for Variant EditContext
        }
    } // END: RenderLayer iterations

    // Set the default modeling variant
    UsdVariantSet modelingVariantSet = usdVariantRootPrim.GetVariantSet("modelingVariant");
    if (modelingVariantSet.IsValid()) {
        modelingVariantSet.SetVariantSelection(defaultModelingVariant);
    }
    return defaultPrim;
}

bool usdWriteJob::needToTraverse(const MDagPath& curDag)
{
    MObject ob = curDag.node();
    // NOTE: Already skipping all intermediate objects
    // skip all intermediate nodes (and their children)
    if (PxrUsdMayaUtil::isIntermediate(ob)) {
        return false;
    }

    // skip nodes that aren't renderable (and their children)

    if (mArgs.excludeInvisible && !PxrUsdMayaUtil::isRenderable(ob)) {
        return false;
    }

    if (!mArgs.exportDefaultCameras && ob.hasFn(MFn::kTransform)) {
        // Ignore transforms of default cameras 
        MString fullPathName = curDag.fullPathName(); 
        if (fullPathName == "|persp" || 
            fullPathName == "|top" || 
            fullPathName == "|front" || 
            fullPathName == "|side") { 
            return false; 
        }
    }

    return true;
}

void usdWriteJob::perFrameCallback(double iFrame)
{
    if (!mArgs.melPerFrameCallback.empty()) {
        MGlobal::executeCommand(mArgs.melPerFrameCallback.c_str(), true);
    }

    if (!mArgs.pythonPerFrameCallback.empty()) {
        MGlobal::executePythonCommand(mArgs.pythonPerFrameCallback.c_str(), true);
    }
}


// write the frame ranges and statistic string on the root
// Also call the post callbacks
void usdWriteJob::postCallback()
{
    if (!mArgs.melPostCallback.empty()) {
        MGlobal::executeCommand(mArgs.melPostCallback.c_str(), true);
    }

    if (!mArgs.pythonPostCallback.empty()) {
        MGlobal::executePythonCommand(mArgs.pythonPostCallback.c_str(), true);
    }
}



PXR_NAMESPACE_CLOSE_SCOPE

