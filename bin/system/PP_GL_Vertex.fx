///////////////////////////////////////////////
//Vertexshader for Postprocessing with OpenGL//
///////////////////////////////////////////////

void main(void)
{
	vec2 Position;
	Position.xy = sign(gl_Vertex.xy);
	gl_Position = vec4(Position.xy, 0.0, 1.0);
	
	Position.xy *= -1;
	gl_TexCoord[0].st = 1.0-(Position.xy * 0.5 + 0.5);
}