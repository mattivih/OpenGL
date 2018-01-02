Shader "Metropolia/Wobble"
{
	Properties
	{
		_Color("Color", Color) = (1, 1, 1, 1)
		_NoiseMap("Noise", 2D) = "white"{}
		_Amount("Amount", Float) = 1.0
	}

	Subshader
	{
		Tags{"RenderType" = "Opaque"}

		CGPROGRAM

		#pragma surface surf Standard fullforwardshadows vertex:vert
		#pragma target 3.0

		struct Input
		{
			float2 uv_MainTex;
		};

		fixed4 _Color;
		sampler2D _NoiseMap;
		float4 _NoiseMap_ST;
		float _Amount;

		void vert(inout appdata_full v)
		{
			_NoiseMap_ST.z += _Time[1];
			_NoiseMap_ST.w += _Time[1];
			v.texcoord.xy = TRANSFORM_TEX(v.texcoord.xy, _NoiseMap);
			float a = tex2Dlod(_NoiseMap, v.texcoord);
			v.vertex.xyz += v.normal * a * _Amount;
		}

		void surf(Input IN, inout SurfaceOutputStandard o)
		{
			o.Albedo = _Color;
		}

		ENDCG
	}
}
