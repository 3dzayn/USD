#ifndef USDMAYA_ARNOLD_SHADER_EXPORT_H
#define USDMAYA_ARNOLD_SHADER_EXPORT_H

#include "pxr/usd/usdAi/aiShader.h"

#include "usdMaya/util.h"

#include <maya/MObject.h>
#include <maya/MDagPath.h>

struct AtNode;

class ArnoldShaderExport {
public:
    ArnoldShaderExport(const UsdStageRefPtr& _stage, UsdTimeCode _time_code,
                       PxrUsdMayaUtil::MDagPathMap<SdfPath>::Type& dag_to_usd);
    ~ArnoldShaderExport();

    static bool is_valid();
private:
    enum TransformAssignment{
        TRANSFORM_ASSIGNMENT_DISABLE,
        TRANSFORM_ASSIGNMENT_BAKE,
        TRANSFORM_ASSIGNMENT_FULL
    };
    std::map<const AtNode*, SdfPath> m_shader_to_usd_path;
    const UsdStageRefPtr& m_stage;
    PxrUsdMayaUtil::MDagPathMap<SdfPath>::Type& m_dag_to_usd;
    SdfPath m_shaders_scope;
    UsdTimeCode m_time_code;
    TransformAssignment m_transform_assignment;

    void export_parameter(const AtNode* arnold_node, UsdAiShader& shader, const char* arnold_param_name, uint8_t arnold_param_type, bool user);
    SdfPath write_arnold_node(const AtNode* arnold_node, SdfPath parent_path);
public:
    SdfPath export_shader(MObject obj);
    void setup_shaders(const MDagPath& dg, const SdfPath& path);
};

#endif
