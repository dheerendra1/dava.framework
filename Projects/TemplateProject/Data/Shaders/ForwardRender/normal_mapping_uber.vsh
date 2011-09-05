attribute vec4 inPosition;
attribute vec2 inTexCoord;

uniform mat4 modelViewProjectionMatrix;
uniform vec4 flatColor;
varying vec4 varColor;
varying vec2 varTexCoord;

void main()
{
	gl_Position = modelViewProjectionMatrix * inPosition;
	varColor = flatColor;
	varTexCoord = inTexCoord;
}
