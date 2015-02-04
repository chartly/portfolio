struct ParticleIn
{
	float2 position : POSITION;
	float4 color : COLOR;
};

struct ParticleOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer cb0 : register(b0)
{
	matrix g_mViewProjection;
	float g_fParticleSize;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

void ParticleVS(inout ParticleIn passthrough) { }

//--------------------------------------------------------------------------------------
// Particle Geometry Shader
//--------------------------------------------------------------------------------------

static const float2 g_positions[4] = { float2(-1, 1), float2(-1, -1), float2(1, 1), float2(1, -1) };

[maxvertexcount(4)]
void ParticleGS(point ParticleIn In[1], inout TriangleStream<ParticleOut> SpriteStream)
{
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		ParticleOut Out = (ParticleOut)0;
		float4 position = float4(In[0].position, 0, 1) + g_fParticleSize * float4(g_positions[i], 0, 0);
		Out.position = mul(position, g_mViewProjection);
		Out.color = In[0].color;
		SpriteStream.Append(Out);
	}
	SpriteStream.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 ParticlePS(ParticleOut In) : SV_Target
{
	return In.color;
}
