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
GLSL Bloom
Effect 2: Horizontal blur, needs PS 2.0
*************************************************************************/


/*uniform vec4 vecValues[2];*/
uniform vec4 vecValues; 
uniform vec4 vecValues1; 
#define blurRadius 5
uniform sampler2D texture1;

void main()
{
   vec4 Color = texture2D(texture1,gl_TexCoord[0].xy);
   vec2 tempTex;
   tempTex.y = gl_TexCoord[0].y;
   for(int i=-blurRadius;i<=blurRadius;i++)
   {
      tempTex.x = gl_TexCoord[0].x+i*vecValues.x;
      Color += texture2D(texture1,tempTex.xy);
   }
   gl_FragColor = Color/(blurRadius*2+1);
} 