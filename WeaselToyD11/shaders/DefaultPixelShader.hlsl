//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

#include "common/ConstantBuffers.hlsl"
#include "common/macroGLSL.hlsl"

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

cbuffer cbCustomisable : register (b2)
{
	// slider colour min -1.0 max 1.0
};

float4 main(PS_INPUT input) : SV_Target
{
	// Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    // Time varying pixel colour
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx);
	
	return vec4(col, 1.0);
}