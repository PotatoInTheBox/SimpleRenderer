#pragma once

// Input data for vertex shader.
struct VertexInput { /* attributes from Mesh */ };
// Output data for vertex shader.
struct VertexOutput { /* clipPos, worldPos, normal, color, etc */ };

// possibly input/output data for fragment shader as well?

// Interface for vertex shader. 
class IVertexShader {
public:
	//virtual VertexOutput run(const VertexInput& in, const Mat4& model, const Mat4& view, const Mat4& proj) = 0;
};

// Interface for fragment shader. 
class IFragmentShader {
public:
	//virtual Color run(const VertexOutput& frag) = 0;
};