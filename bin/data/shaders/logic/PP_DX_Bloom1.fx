/***********************************************************************
Copyright (c) 2008 Nils Daumann
 
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:
 
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*************************************************************************/

/************************************************************************
HLSL Bloom
Effect 1: Filtering out dark Colors, needs PS 1.4
*************************************************************************/


float4 vecValues[2];
sampler2D colorMap : register(s0);

float4 main( float2 Tex:TEXCOORD0 ) : COLOR0 
{
	float4 Color = tex2D(colorMap,Tex.xy);
	if(dot(Color.rgb,float3(0.299f,0.587f,0.114f)) < vecValues[0].x)
	{
		Color.rgb = 0;
	}
	return Color;
}