attribute vec4 inPosition;

uniform mat4 modelViewProjectionMatrix;
uniform vec4 flatColor;
varying vec4 varColor;

void main()
{
	gl_Position = modelViewProjectionMatrix * inPosition;
	varColor = flatColor;
}
