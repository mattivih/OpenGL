Shader "Metropolia/Grass" 
{
	Properties {
		_MainTex ("Texture", 2D) = "white" {}
		_WeightMap("Weight map", 2D) = "white" {}
		_Noise("Noise", Float) = 1
		_Speed("Speed", Float) = 1
		_Scale("Scale", Float) = 1
	}
	SubShader 
	{
		Tags
		{ 
			"Queue" = "Transparent"
			"RenderType" = "Transparent"
		}
				
		CGPROGRAM
		
		#pragma surface surf Standard fullforwardshadows vertex:vert alpha:fade
		#pragma target 3.0

		#include "noiseSimplex.cginc"

		struct Input
		{
			float2 uv_MainTex;
		};

		sampler2D _MainTex;
		sampler2D _WeightMap;
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
			//float4 uv = v.texcoord;
			//uv.xyz += val;
			float w = tex2Dlod(_WeightMap, v.texcoord).r;
			float nx = snoise(float3(val, v.texcoord.x, v.texcoord.y));
			float ny = snoise(float3(v.texcoord.x, val, v.texcoord.y));
			float nz = snoise(float3(v.texcoord.x, v.texcoord.y, val));
			float3 p = deform(float3(nx, ny, nz) * _Scale, _Noise);

			v.vertex.xyz += p * w;
		}

		void surf(Input IN, inout SurfaceOutputStandard o)
		{
			fixed4 c = tex2D(_MainTex, IN.uv_MainTex);
			o.Albedo = c;
			o.Alpha = c.a;
		}

		ENDCG
	}
}
