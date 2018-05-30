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
#include "usdMaya/usdExport.h"

#include "usdMaya/usdWriteJob.h"
#include "usdMaya/util.h"
#include "usdMaya/shadingModeRegistry.h"

#include <maya/MFileObject.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MAnimControl.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MComputation.h>
#include <maya/MObjectArray.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>
#include <maya/MTime.h>

#include "pxr/usd/usdGeom/tokens.h"
#include "JobArgs.h"

PXR_NAMESPACE_OPEN_SCOPE


usdExport::usdExport()
{
}

usdExport::~usdExport()
{
}

MSyntax usdExport::createSyntax()
{
    MSyntax syntax;

    // These flags correspond to entries in JobExportArgs::GetDefaultDictionary.
    syntax.addFlag("-mt",
                   PxrUsdExportJobArgsTokens->mergeTransformAndShape.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-ein",
                   PxrUsdExportJobArgsTokens->exportInstances.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-eri",
                   PxrUsdExportJobArgsTokens->exportRefsAsInstanceable.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-dsp",
                   PxrUsdExportJobArgsTokens->exportDisplayColor.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-shd",
                   PxrUsdExportJobArgsTokens->shadingMode.GetText() ,
                   MSyntax::kString);
    syntax.addFlag("-uvs",
                   PxrUsdExportJobArgsTokens->exportUVs.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-mcs",
                   PxrUsdExportJobArgsTokens->exportMaterialCollections
                       .GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-mcp",
                   PxrUsdExportJobArgsTokens->materialCollectionsPath.GetText(),
                   MSyntax::kString);
    syntax.addFlag("-cbb",
                   PxrUsdExportJobArgsTokens->exportCollectionBasedBindings
                       .GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-nnu",
                   PxrUsdExportJobArgsTokens->normalizeNurbs.GetText() ,
                   MSyntax::kBoolean);
    syntax.addFlag("-cls",
                   PxrUsdExportJobArgsTokens->exportColorSets.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-dms",
                   PxrUsdExportJobArgsTokens->defaultMeshScheme.GetText(),
                   MSyntax::kString);
    syntax.addFlag("-vis",
                   PxrUsdExportJobArgsTokens->exportVisibility.GetText(),
                   MSyntax::kBoolean);
    syntax.addFlag("-skn",
                   PxrUsdExportJobArgsTokens->exportSkin.GetText(),
                   MSyntax::kString);
    syntax.addFlag("-psc",
                   PxrUsdExportJobArgsTokens->parentScope.GetText(),
                   MSyntax::kString);
    syntax.addFlag("-ro",
                   PxrUsdExportJobArgsTokens->renderableOnly.GetText(),
                   MSyntax::kNoArg);
    syntax.addFlag("-dc",
                   PxrUsdExportJobArgsTokens->defaultCameras.GetText(),
                   MSyntax::kNoArg);
    syntax.addFlag("-rlm",
                   PxrUsdExportJobArgsTokens->renderLayerMode.GetText(),
                   MSyntax::kString);
    syntax.addFlag("-k",
                   PxrUsdExportJobArgsTokens->kind.GetText(),
                   MSyntax::kString);

    syntax.addFlag("-chr",
                   PxrUsdExportJobArgsTokens->chaser.GetText(),
                   MSyntax::kString);
    syntax.makeFlagMultiUse(PxrUsdExportJobArgsTokens->chaser.GetText());

    syntax.addFlag("-cha",
                   PxrUsdExportJobArgsTokens->chaserArgs.GetText(),
                   MSyntax::kString, MSyntax::kString, MSyntax::kString);
    syntax.makeFlagMultiUse(PxrUsdExportJobArgsTokens->chaserArgs.GetText());

    syntax.addFlag("-mfc",
                   PxrUsdExportJobArgsTokens->melPerFrameCallback.GetText(),
                   MSyntax::kNoArg);
    syntax.addFlag("-mpc",
                   PxrUsdExportJobArgsTokens->melPostCallback.GetText(),
                   MSyntax::kNoArg);
    syntax.addFlag("-pfc",
                   PxrUsdExportJobArgsTokens->pythonPerFrameCallback.GetText(),
                   MSyntax::kString);
    syntax.addFlag("-ppc",
                   PxrUsdExportJobArgsTokens->pythonPostCallback.GetText(),
                   MSyntax::kString);

    // These are additional flags under our control.
    syntax.addFlag("-fr", "-frameRange", MSyntax::kDouble, MSyntax::kDouble);
    syntax.addFlag("-fs", "-frameSample", MSyntax::kDouble);
    syntax.makeFlagMultiUse("-frameSample");

    syntax.addFlag("-a", "-append", MSyntax::kBoolean);
    syntax.addFlag("-f", "-file", MSyntax::kString);
    syntax.addFlag("-sl", "-selection", MSyntax::kNoArg);

    syntax.addFlag("-v", "-verbose", MSyntax::kNoArg);

    syntax.enableQuery(false);
    syntax.enableEdit(false);

    syntax.setObjectType(MSyntax::kSelectionList);
    syntax.setMinObjects(0);

    return syntax;
}


void* usdExport::creator()
{
    return new usdExport();
}


MStatus usdExport::doIt(const MArgList & args)
{
try
{
    MStatus status;

    MArgDatabase argData(syntax(), args, &status);

    // Check that all flags were valid
    if (status != MS::kSuccess) {
        MGlobal::displayError("Invalid parameters detected.  Exiting.");
        return status;
    }

    // Read all of the dictionary args first.
    const VtDictionary userArgs = PxrUsdMayaUtil::GetDictionaryFromArgDatabase(
            argData, JobExportArgs::GetDefaultDictionary());

    // Now read all of the other args that are specific to this command.
    bool verbose = argData.isFlagSet("verbose");
    bool append = false;
    std::string fileName;

    if (argData.isFlagSet("append")) {
        argData.getFlagArgument("append", 0, append);
    }

    if (argData.isFlagSet("file"))
    {
        // Get the value
        MString tmpVal;
        argData.getFlagArgument("file", 0, tmpVal);

        // resolve the path into an absolute path
        MFileObject absoluteFile;
        absoluteFile.setRawFullName(tmpVal);
        absoluteFile.setRawFullName( absoluteFile.resolvedFullName() ); // Make sure an absolute path
        fileName = absoluteFile.resolvedFullName().asChar();

        if (fileName.empty()) {
            fileName = tmpVal.asChar();
        }
        MGlobal::displayInfo(MString("Saving as ") + MString(fileName.c_str()));
    }
    else {
        MString error = "-file not specified.";
        MGlobal::displayError(error);
        return MS::kFailure;
    }

    if (fileName.empty()) {
        return MS::kFailure;
    }

    // If you provide a frame range we consider this an anim
    // export even if start and end are the same
    GfInterval timeInterval;
    if (argData.isFlagSet("frameRange")) {
        double startTime = 1;
        double endTime = 1;
        argData.getFlagArgument("frameRange", 0, startTime);
        argData.getFlagArgument("frameRange", 1, endTime);
        if (startTime > endTime) {
            // If the user accidentally set start > end, resync to the closed
            // interval with the single start point.
            timeInterval = GfInterval(startTime);
        }
        else {
            // Use the user's interval as-is.
            timeInterval = GfInterval(startTime, endTime);
        }
    } else {
        // No animation, so empty interval.
        timeInterval = GfInterval();
    }

    std::set<double> frameSamples;
    unsigned int numFrameSamples = argData.numberOfFlagUses("frameSample");
    for (unsigned int i = 0; i < numFrameSamples; ++i) {
        MArgList tmpArgList;
        argData.getFlagArgumentList("frameSample", i, tmpArgList);
        frameSamples.insert(tmpArgList.asDouble(0));
    }

    if (frameSamples.empty()) {
        frameSamples.insert(0.0);
    }

    // Get the objects to export as a MSelectionList
    MSelectionList objSelList;
    if (argData.isFlagSet("selection")) {
        MGlobal::getActiveSelectionList(objSelList);
    }
    else {
        argData.getObjects(objSelList);

        // If no objects specified, then get all objects at DAG root
        if (objSelList.isEmpty()) {
            objSelList.add("|*", true);
        }
    }

    // Convert selection list to jobArgs dagPaths
    PxrUsdMayaUtil::ShapeSet dagPaths;
    for (unsigned int i=0; i < objSelList.length(); i++) {
        MDagPath dagPath;
        status = objSelList.getDagPath(i, dagPath);
        if (status == MS::kSuccess)
        {
            dagPaths.insert(dagPath);
        }
    }

    JobExportArgs jobArgs = JobExportArgs::CreateFromDictionary(
            userArgs, dagPaths, timeInterval);

    // Create WriteJob object
    usdWriteJob usdWriteJob(jobArgs);

    MComputation computation;
    computation.beginComputation();

    // Create stage and process static data
    if (usdWriteJob.beginJob(fileName, append)) {
        if (!jobArgs.timeInterval.IsEmpty()) {
            const MTime oldCurTime = MAnimControl::currentTime();
            for (double i = jobArgs.timeInterval.GetMin();
                    jobArgs.timeInterval.Contains(i);
                    i += 1.0) {
                for (double sampleTime : frameSamples) {
                    const double actualTime = i + sampleTime;
                    if (verbose) {
                        MString info;
                        info = actualTime;
                        MGlobal::displayInfo(info);
                    }
                    MGlobal::viewFrame(actualTime);
                    // Process per frame data
                    usdWriteJob.evalJob(actualTime);
                    if (computation.isInterruptRequested()) {
                        break;
                    }
                }
            }

            // Set the time back.
            MGlobal::viewFrame(oldCurTime);
        }

        // Finalize the export, close the stage
        usdWriteJob.endJob();
    } else {
        computation.endComputation();
        return MS::kFailure;
    }

    computation.endComputation();

    return MS::kSuccess;
} // end of try block
catch (std::exception & e)
{
    MString theError("std::exception encountered: ");
    theError += e.what();
    MGlobal::displayError(theError);
    return MS::kFailure;
}
} // end of function

PXR_NAMESPACE_CLOSE_SCOPE

