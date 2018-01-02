Shader "Metropolia/SinPlane"
{
	Properties
	{
		_Color("Color", Color) = (1, 1, 1, 1)
		_Scale("Scale", Float) = 10.0
	}

	SubShader
	{
		Tags {"RendererType" = "Opaque"}

		CGPROGRAM

		#pragma surface surf Standard fullforwardshadows vertex:vert
		#pragma target 3.0

		struct Input
		{
			float3 worldPos;
		};

		fixed4 _Color;
		float _Scale;

		void vert(inout appdata_full v)
		{
			v.vertex.y += sin(v.texcoord.x * _Scale) * cos(v.texcoord.y * _Scale);
		}

		void surf(Input IN, inout SurfaceOutputStandard o)
		{
			fixed4 c = fixed4(IN.worldPos.y, 0, 0, 1);
			o.Albedo = c;
			o.Alpha = 1.0;
		}

		ENDCG
	}
}