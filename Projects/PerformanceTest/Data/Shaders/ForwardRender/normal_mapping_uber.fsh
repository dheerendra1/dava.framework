#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#endif

uniform sampler2D sampler2d;
varying vec4 varColor;
varying vec2 varTexCoord;

void main()
{
	gl_FragColor = texture2D(sampler2d, varTexCoord) * varColor;
}
