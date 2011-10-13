#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#endif

uniform sampler2D texture;
uniform sampler2D detailTexture;

varying vec2 varTexCoord;
varying vec2 varDetailCoord;

void main()
{
    gl_FragColor = texture2D(texture, varTexCoord) * texture2D(detailTexture, varDetailCoord) * 2.0; //
}
