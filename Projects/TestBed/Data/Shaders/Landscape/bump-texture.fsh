#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#endif

uniform sampler2D texture;
uniform sampler2D detailTexture;

varying vec4 varColor;
varying vec2 varTexCoord;
varying vec2 varDetailCoord;
varying vec3 varCameraToVertex;


void main()
{
    vec3 normal = texture2D(detailTexture, varDetailCoord).xyz * 2.0 - 1.0;
    vec4 color = texture2D(texture, varTexCoord);
    vec3 light = vec3(0.0, 0.0, 1.0);
    float intensity = dot(normal, varCameraToVertex);
    gl_FragColor = texture2D(texture, varTexCoord) * intensity; //
    
//  vec4 detailColor = texture2D(detailTexture, varDetailCoord);
//  vec4 color = texture2D(texture, varTexCoord);
//	gl_FragColor = color * detailColor * 2.0;
}
