//
// Copyright 2019 Pixar
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
#include "pxr/imaging/glf/glew.h"
#include "pxr/imaging/glf/diagnostic.h"
#include "pxr/imaging/glf/drawTarget.h"

#include "pxr/imaging/hdx/ambientOcclusionTask.h"

#include "pxr/imaging/hd/perfLog.h"
#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/imaging/hd/renderIndex.h"
#include "pxr/imaging/hd/renderPass.h"
#include "pxr/imaging/hd/renderPassState.h"
#include "pxr/imaging/hd/rprimCollection.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/hd/vtBufferSource.h"

#include "pxr/imaging/hdSt/renderPassShader.h"
#include "pxr/imaging/hdSt/renderPassState.h"
#include "pxr/imaging/hdSt/renderDelegate.h"
#include "pxr/imaging/hdSt/imageShaderRenderPass.h"
#include "pxr/imaging/hdSt/tokens.h"

#include "pxr/imaging/hdx/package.h"
#include "pxr/imaging/hdx/utils.h"

#include <random>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (hdxAoKernel)
);

namespace {

class HdxAmbientOcclusionRenderPassShader : public HdStRenderPassShader {
public:
    HdxAmbientOcclusionRenderPassShader(int numSamples) :
        HdStRenderPassShader(HdxPackageAmbientOcclusionImageShader()),
        _numSamples(numSamples)
    {
        // The hash of this shader is constant, no custom bindings and the
        // input parameters are constant.
        _hash = HdStRenderPassShader::ComputeHash();
        boost::hash_combine(_hash, numSamples);
    }

    std::string GetSource(const TfToken& shaderStageKey) const override
    {
        const auto src = HdStRenderPassShader::GetSource(shaderStageKey);

        std::stringstream defines;
        defines << "#define AO_SAMPLES "
                << _numSamples
                << "\n";

        return defines.str() + src;
    }

    ID ComputeHash() const override
    {
        return _hash;
    }

    void BindResources(const HdSt_ResourceBinder& binder, int program) override {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _depthTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _colorTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _normalTex);
        HdStRenderPassShader::BindResources(binder, program);
    }

    inline void SetDepthTexture(GLuint tex) {
        _depthTex = tex;
    }

    inline void SetColorTexture(GLuint tex) {
        _colorTex = tex;
    }

    inline void SetNormalTexture(GLuint tex) {
        _normalTex = tex;
    }

    ~HdxAmbientOcclusionRenderPassShader() override = default;
private:
    HdxAmbientOcclusionRenderPassShader()                                                       = delete;
    HdxAmbientOcclusionRenderPassShader(const HdxAmbientOcclusionRenderPassShader&)             = delete;
    HdxAmbientOcclusionRenderPassShader(HdxAmbientOcclusionRenderPassShader&&)                  = delete;
    HdxAmbientOcclusionRenderPassShader& operator=(const  HdxAmbientOcclusionRenderPassShader&) = delete;
    HdxAmbientOcclusionRenderPassShader& operator=(HdxAmbientOcclusionRenderPassShader&&)       = delete;

    const int _numSamples;
    ID _hash;
    int _depthTex;
    int _colorTex;
    int _normalTex;
};

// The sample should conform to poission disc sampling.
// Once we have the normal available, this becomes a bit easier.
VtArray<float> _GenerateSamplingKernel(const int numPoints)
{
    std::ranlux24 engine1(42);
    std::ranlux24 engine2(137);

    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    VtArray<float> ret;
    ret.reserve(numPoints * 2);
    for (auto i = decltype(numPoints){0}; i < numPoints; i += 1) {
        const auto angle = distribution(engine1) * M_PI * 2.0f;
        const auto distance = sqrtf(distribution(engine2));
        ret.push_back(distance * sinf(angle));
        ret.push_back(distance * cosf(angle));
    }
    return ret;
}

}

HdxAmbientOcclusionTask::HdxAmbientOcclusionTask(
    HdSceneDelegate* delegate, const SdfPath& id) : HdTask(id)
{

}

HdxAmbientOcclusionTask::~HdxAmbientOcclusionTask()
{

}

void HdxAmbientOcclusionTask::Sync(HdSceneDelegate* delegate,
          HdTaskContext* ctx,
          HdDirtyBits* dirtyBits)
{
    HD_TRACE_FUNCTION();
    *dirtyBits = HdChangeTracker::Clean;
}

void HdxAmbientOcclusionTask::Prepare(HdTaskContext* ctx,
                                      HdRenderIndex* renderIndex)
{
    auto* renderDelegate = renderIndex->GetRenderDelegate();
    if (!TF_VERIFY(dynamic_cast<HdStRenderDelegate*>(renderDelegate),
                   "OIT Task only works with HdSt")) {
        return;
    }

    auto enableAo = renderDelegate
        ->GetRenderSetting(HdStRenderSettingsTokens->enableAo);
    if (!TF_VERIFY(enableAo.IsHolding<bool>(),
                   "Enable Ambient Occlusion is not a bool!")) {
        return;
    }
    if (!TF_VERIFY(enableAo.UncheckedGet<bool>(),
                   "Enable Ambient Occlusion is false, "
                   "yet the task is running.")) {
        return;
    }

    auto aoNumSamples = renderDelegate
        ->GetRenderSetting(HdStRenderSettingsTokens->aoNumSamples);
    if (!TF_VERIFY(aoNumSamples.IsHolding<int>(),
                   "Ambient Occlusion num samples is not an integer!")) {
        return;
    }
    const auto numSamples = std::max(1, aoNumSamples.UncheckedGet<int>());
    auto rebuildShader = false;
    if (numSamples != _numSamples) {
        _numSamples = numSamples;
        rebuildShader = true;
    }

    const auto& resourceRegistry = renderIndex->GetResourceRegistry();

    auto buildKernel = [&] () {
        HdBufferSpecVector kernelSpecs;
        kernelSpecs.push_back(
            HdBufferSpec(
                _tokens->hdxAoKernel,
                HdTupleType {HdTypeFloat, 1})
        );

        _kernelBar = resourceRegistry->AllocateSingleBufferArrayRange(
            _tokens->hdxAoKernel,
            kernelSpecs,
            HdBufferArrayUsageHint()
        );

        HdBufferSourceSharedPtr kernelSource(
            new HdVtBufferSource(
                _tokens->hdxAoKernel,
                VtValue(_GenerateSamplingKernel(numSamples))
            )
        );
        resourceRegistry->AddSource(_kernelBar, kernelSource);

        _renderPassShader->AddBufferBinding(
            HdBindingRequest(
                HdBinding::SSBO,
                _tokens->hdxAoKernel,
                _kernelBar,
                false /* interleave */
            ));
    };

    if (!_renderPass) {
        HdRprimCollection collection;

        _renderPass.reset(
            new HdSt_ImageShaderRenderPass(renderIndex, collection));

        // To avoid having to access the color buffer for manipuation, use
        // OpenGL's blending pipeline to multiply the color buffer with the
        // alpha value of our image shader, which is the inverse of the
        // ambient occlusion factor.
        _renderPassState.reset(new HdStRenderPassState());
        _renderPassState->SetEnableDepthMask(false);
        _renderPassState->SetColorMask(HdRenderPassState::ColorMaskRGBA);
        _renderPassState->SetBlendEnabled(true);
        _renderPassState->SetBlend(
            HdBlendOp::HdBlendOpAdd,
            HdBlendFactor::HdBlendFactorOne,
            HdBlendFactor::HdBlendFactorOneMinusSrcAlpha,
            HdBlendOp::HdBlendOpAdd,
            HdBlendFactor::HdBlendFactorOne,
            HdBlendFactor::HdBlendFactorOne);

        _renderPassShader.reset(new HdxAmbientOcclusionRenderPassShader(
            numSamples));

        _renderPassState->SetRenderPassShader(_renderPassShader);
        buildKernel();
        _renderPass->Prepare(GetRenderTags());
    } else if(rebuildShader) {
        _renderPassShader.reset(new HdxAmbientOcclusionRenderPassShader(
            _numSamples));
        _renderPassState->SetRenderPassShader(_renderPassShader);
        buildKernel();
        _renderPass->Prepare(GetRenderTags());
    }
}

void HdxAmbientOcclusionTask::Execute(HdTaskContext* ctx)
{
    HD_TRACE_FUNCTION();
    HF_MALLOC_TAG_FUNCTION();

    if (!TF_VERIFY(_renderPassState)) return;

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION
                   , 0
                   , -1
                   , "Ambient Occlusion Rendering");

    GLint drawFramebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebuffer);

    const auto screenSize = HdxUtils::GetScreenSize();

    auto drawTarget = GlfDrawTarget::New(screenSize
                                       , false /* request MSAA */);

    drawTarget->Bind();
    drawTarget->AddAttachment("depth"
                            , GL_DEPTH_COMPONENT
                            , GL_FLOAT
                            , GL_DEPTH_COMPONENT32F);
    drawTarget->AddAttachment("color"
                            , GL_RGBA
                            , GL_FLOAT
                            , GL_RGBA16F);
    drawTarget->AddAttachment("normal"
                            , GL_RGBA
                            , GL_FLOAT
                            , GL_RGBA16F);
    drawTarget->DrawBuffers();

    auto framebuffer = drawTarget->GetFramebufferId();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, drawFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    // Blit framebuffers don't copy all the buffers at once. Need to do one by
    // one. We copy depth with the first color attachment copy.
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, screenSize[0], screenSize[1],
                      0, 0, screenSize[0], screenSize[1],
                      GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, screenSize[0], screenSize[1],
                      0, 0, screenSize[0], screenSize[1],
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, drawFramebuffer);
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);
    GLF_POST_PENDING_GL_ERRORS();

    auto* shader = static_cast<HdxAmbientOcclusionRenderPassShader*>(
        _renderPassShader.get());
    shader->SetDepthTexture(drawTarget->GetAttachment("depth")->GetGlTextureName());
    shader->SetColorTexture(drawTarget->GetAttachment("color")->GetGlTextureName());
    shader->SetNormalTexture(drawTarget->GetAttachment("normal")->GetGlTextureName());

    _renderPassState->Bind();

    glDisable(GL_DEPTH_TEST);

    _renderPass->Execute(_renderPassState, GetRenderTags());

    glEnable(GL_DEPTH_TEST);

    _renderPassState->Unbind();

    drawTarget->Bind();
    drawTarget->ClearAttachments();
    drawTarget = nullptr;
    glPopDebugGroup();
}

std::ostream& operator<<(
    std::ostream& out, const HdxAmbientOcclusionTaskParams& pv)
{
    out << "AmbientOcclusionTask Params: (...) "
        << pv.enable;
    return out;
}

bool operator==(
    const HdxAmbientOcclusionTaskParams& lhs,
    const HdxAmbientOcclusionTaskParams& rhs)
{
    return lhs.enable == rhs.enable;
}

bool operator!=(
    const HdxAmbientOcclusionTaskParams& lhs,
    const HdxAmbientOcclusionTaskParams& rhs)
{
    return !(lhs == rhs);
}


PXR_NAMESPACE_CLOSE_SCOPE