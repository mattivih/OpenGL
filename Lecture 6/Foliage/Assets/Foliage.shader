Shader "Metropolia/Foliage"
{
	Properties
	{
		_MainTex("Texture", 2D) = "white" {}
	_Noise("Noise", Float) = 1
		_Speed("Speed", Float) = 1
		_Scale("Scale", Float) = 1
		_Cutoff("Alpha cutoff", Range(0, 1)) = 0.5
	}

		SubShader
	{
		Tags
	{
		"Queue" = "AlphaTest"
		"IgnoreProjection" = "True"
		"RenderType" = "TransparentCutoff"
	}
		Cull off

		CGPROGRAM

#pragma surface surf Standard fullforwardshadows vertex:vert alphatest:_Cutoff
#pragma target 3.0

#include "noiseSimplex.cginc"

		struct Input
	{
		float2 uv_MainTex;
	};

	sampler2D _MainTex;
	float _Noise;
	float _Speed;
	float _Scale;

	float3 deform(float3 n, float na)
	{
		return n * na * 0.001;
	}

	void vert(inout appdata_full v)
	{
		float val = _Time[1] * _Speed;
		float w = v.color.r;
		float pha = v.color.b;
		float nx = snoise(float3(val + pha, 0, 0));
		float ny = snoise(float3(0, val + pha, 0));
		float nz = snoise(float3(0, 0, val + pha));
		float3 p = deform(float3(nx, ny, nz) * _Scale, _Noise);

		v.vertex.xyz += p * w;
	}

	void surf(Input IN, inout SurfaceOutputStandard o)
	{
		fixed4 c = tex2D(_MainTex, IN.uv_MainTex);
		o.Albedo = c.rgb;
		o.Alpha = c.a;
	}

	ENDCG
	}
}