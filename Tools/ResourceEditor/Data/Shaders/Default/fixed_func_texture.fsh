#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#else
#define lowp
#define highp
#define mediump
#endif

uniform sampler2D sampler2d;
varying lowp vec4 varColor;
varying mediump vec2 varTexCoord;

void main()
{
    lowp vec4 texColor = texture2D(sampler2d, varTexCoord);
#ifdef ALPHA_TEST_ENABLED
    if (texColor.a < 0.9)
        discard;
#endif
	gl_FragColor = texColor * varColor;
}
