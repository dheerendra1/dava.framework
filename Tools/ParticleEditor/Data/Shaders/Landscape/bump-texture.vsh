attribute vec4 inPosition;
attribute vec2 inTexCoord;

uniform mat4 modelViewProjectionMatrix;
uniform vec4 flatColor;
uniform vec3 cameraPosition;
varying vec4 varColor;
varying vec2 varTexCoord;
varying vec2 varDetailCoord;
varying vec3 varCameraToVertex;

void main()
{
	gl_Position = modelViewProjectionMatrix * inPosition;
	varColor = flatColor * flatColor.a;
	varTexCoord = inTexCoord;
	varDetailCoord = inTexCoord * 30.0;
	varCameraToVertex = normalize(cameraPosition - inPosition.xyz);
}
