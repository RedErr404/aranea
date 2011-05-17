///////////////////////////////////////////////
//Vertexshader for Postprocessing with DX9.0c//
///////////////////////////////////////////////
void main(	in float4 inPos : POSITION,
				out float4 outPos : POSITION,
				out float2 outTexCoords : TEXCOORD0)
{
	outPos = inPos;
	
	inPos.y *= -1;
	outTexCoords = inPos.xy*0.5+0.5;
}