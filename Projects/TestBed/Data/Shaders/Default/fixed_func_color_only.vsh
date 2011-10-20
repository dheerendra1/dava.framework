attribute vec4 inPosition;

uniform mat4 modelViewProjectionMatrix;
uniform lowp vec4 flatColor;
varying lowp vec4 varColor;

void main()
{
	gl_Position = modelViewProjectionMatrix * inPosition;
	varColor = flatColor * flatColor.a;
}
