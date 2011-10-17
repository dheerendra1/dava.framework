#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#endif

uniform sampler2D sampler2d;
varying lowp vec4 varColor;
varying mediump vec2 varTexCoord;

void main()
{
    lowp vec4 texColor = texture2D(sampler2d, varTexCoord);
	gl_FragColor = texColor * varColor;
}
