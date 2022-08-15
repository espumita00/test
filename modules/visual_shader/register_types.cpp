/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "register_types.h"

#include "visual_shader.h"
#include "vs_nodes/visual_shader_nodes.h"
#include "vs_nodes/visual_shader_particle_nodes.h"
#include "vs_nodes/visual_shader_sdf_nodes.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor/visual_shader_editor_plugin.h"
#endif // TOOLS_ENABLED

void initialize_visual_shader_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<VisualShader>();
	ClassDB::register_abstract_class<VisualShaderNode>();
	ClassDB::register_class<VisualShaderNodeCustom>();
	ClassDB::register_class<VisualShaderNodeInput>();
	ClassDB::register_abstract_class<VisualShaderNodeOutput>();
	ClassDB::register_abstract_class<VisualShaderNodeResizableBase>();
	ClassDB::register_abstract_class<VisualShaderNodeGroupBase>();
	ClassDB::register_abstract_class<VisualShaderNodeConstant>();
	ClassDB::register_abstract_class<VisualShaderNodeVectorBase>();
	ClassDB::register_class<VisualShaderNodeComment>();
	ClassDB::register_class<VisualShaderNodeFloatConstant>();
	ClassDB::register_class<VisualShaderNodeIntConstant>();
	ClassDB::register_class<VisualShaderNodeBooleanConstant>();
	ClassDB::register_class<VisualShaderNodeColorConstant>();
	ClassDB::register_class<VisualShaderNodeVec2Constant>();
	ClassDB::register_class<VisualShaderNodeVec3Constant>();
	ClassDB::register_class<VisualShaderNodeVec4Constant>();
	ClassDB::register_class<VisualShaderNodeTransformConstant>();
	ClassDB::register_class<VisualShaderNodeFloatOp>();
	ClassDB::register_class<VisualShaderNodeIntOp>();
	ClassDB::register_class<VisualShaderNodeVectorOp>();
	ClassDB::register_class<VisualShaderNodeColorOp>();
	ClassDB::register_class<VisualShaderNodeTransformOp>();
	ClassDB::register_class<VisualShaderNodeTransformVecMult>();
	ClassDB::register_class<VisualShaderNodeFloatFunc>();
	ClassDB::register_class<VisualShaderNodeIntFunc>();
	ClassDB::register_class<VisualShaderNodeVectorFunc>();
	ClassDB::register_class<VisualShaderNodeColorFunc>();
	ClassDB::register_class<VisualShaderNodeTransformFunc>();
	ClassDB::register_class<VisualShaderNodeUVFunc>();
	ClassDB::register_class<VisualShaderNodeUVPolarCoord>();
	ClassDB::register_class<VisualShaderNodeDotProduct>();
	ClassDB::register_class<VisualShaderNodeVectorLen>();
	ClassDB::register_class<VisualShaderNodeDeterminant>();
	ClassDB::register_class<VisualShaderNodeDerivativeFunc>();
	ClassDB::register_class<VisualShaderNodeClamp>();
	ClassDB::register_class<VisualShaderNodeFaceForward>();
	ClassDB::register_class<VisualShaderNodeOuterProduct>();
	ClassDB::register_class<VisualShaderNodeSmoothStep>();
	ClassDB::register_class<VisualShaderNodeStep>();
	ClassDB::register_class<VisualShaderNodeVectorDistance>();
	ClassDB::register_class<VisualShaderNodeVectorRefract>();
	ClassDB::register_class<VisualShaderNodeMix>();
	ClassDB::register_class<VisualShaderNodeVectorCompose>();
	ClassDB::register_class<VisualShaderNodeTransformCompose>();
	ClassDB::register_class<VisualShaderNodeVectorDecompose>();
	ClassDB::register_class<VisualShaderNodeTransformDecompose>();
	ClassDB::register_class<VisualShaderNodeTexture>();
	ClassDB::register_class<VisualShaderNodeCurveTexture>();
	ClassDB::register_class<VisualShaderNodeCurveXYZTexture>();
	ClassDB::register_abstract_class<VisualShaderNodeSample3D>();
	ClassDB::register_class<VisualShaderNodeTexture2DArray>();
	ClassDB::register_class<VisualShaderNodeTexture3D>();
	ClassDB::register_class<VisualShaderNodeCubemap>();
	ClassDB::register_abstract_class<VisualShaderNodeParameter>();
	ClassDB::register_class<VisualShaderNodeParameterRef>();
	ClassDB::register_class<VisualShaderNodeFloatParameter>();
	ClassDB::register_class<VisualShaderNodeIntParameter>();
	ClassDB::register_class<VisualShaderNodeBooleanParameter>();
	ClassDB::register_class<VisualShaderNodeColorParameter>();
	ClassDB::register_class<VisualShaderNodeVec2Parameter>();
	ClassDB::register_class<VisualShaderNodeVec3Parameter>();
	ClassDB::register_class<VisualShaderNodeVec4Parameter>();
	ClassDB::register_class<VisualShaderNodeTransformParameter>();
	ClassDB::register_abstract_class<VisualShaderNodeTextureParameter>();
	ClassDB::register_class<VisualShaderNodeTexture2DParameter>();
	ClassDB::register_class<VisualShaderNodeTextureParameterTriplanar>();
	ClassDB::register_class<VisualShaderNodeTexture2DArrayParameter>();
	ClassDB::register_class<VisualShaderNodeTexture3DParameter>();
	ClassDB::register_class<VisualShaderNodeCubemapParameter>();
	ClassDB::register_class<VisualShaderNodeLinearSceneDepth>();
	ClassDB::register_class<VisualShaderNodeIf>();
	ClassDB::register_class<VisualShaderNodeSwitch>();
	ClassDB::register_class<VisualShaderNodeFresnel>();
	ClassDB::register_class<VisualShaderNodeExpression>();
	ClassDB::register_class<VisualShaderNodeGlobalExpression>();
	ClassDB::register_class<VisualShaderNodeIs>();
	ClassDB::register_class<VisualShaderNodeCompare>();
	ClassDB::register_class<VisualShaderNodeMultiplyAdd>();
	ClassDB::register_class<VisualShaderNodeBillboard>();
	ClassDB::register_class<VisualShaderNodeDistanceFade>();
	ClassDB::register_class<VisualShaderNodeProximityFade>();
	ClassDB::register_class<VisualShaderNodeRandomRange>();
	ClassDB::register_class<VisualShaderNodeRemap>();
	ClassDB::register_abstract_class<VisualShaderNodeVarying>();
	ClassDB::register_class<VisualShaderNodeVaryingSetter>();
	ClassDB::register_class<VisualShaderNodeVaryingGetter>();

	ClassDB::register_class<VisualShaderNodeSDFToScreenUV>();
	ClassDB::register_class<VisualShaderNodeScreenUVToSDF>();
	ClassDB::register_class<VisualShaderNodeTextureSDF>();
	ClassDB::register_class<VisualShaderNodeTextureSDFNormal>();
	ClassDB::register_class<VisualShaderNodeSDFRaymarch>();

	ClassDB::register_class<VisualShaderNodeParticleOutput>();
	ClassDB::register_abstract_class<VisualShaderNodeParticleEmitter>();
	ClassDB::register_class<VisualShaderNodeParticleSphereEmitter>();
	ClassDB::register_class<VisualShaderNodeParticleBoxEmitter>();
	ClassDB::register_class<VisualShaderNodeParticleRingEmitter>();
	ClassDB::register_class<VisualShaderNodeParticleMeshEmitter>();
	ClassDB::register_class<VisualShaderNodeParticleMultiplyByAxisAngle>();
	ClassDB::register_class<VisualShaderNodeParticleConeVelocity>();
	ClassDB::register_class<VisualShaderNodeParticleRandomness>();
	ClassDB::register_class<VisualShaderNodeParticleAccelerator>();
	ClassDB::register_class<VisualShaderNodeParticleEmit>();
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		Ref<EditorInspectorVisualShaderModePlugin> smp;
		smp.instantiate();
		EditorInspector::add_inspector_plugin(smp);
		Ref<VisualShaderConversionPlugin> vshader_convert;
		vshader_convert.instantiate();
		EditorNode::get_singleton()->add_resource_conversion_plugin(vshader_convert);
		// Hack to ensure the linker keeps these unregistered editor classes.
		VisualShaderConversionPlugin();
		VisualShaderEditor();
	}
#endif // TOOLS_ENABLED
}

void uninitialize_visual_shader_module(ModuleInitializationLevel p_level) {}
