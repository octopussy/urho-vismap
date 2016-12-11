#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"

varying vec2 vScreenPos;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vScreenPos = GetQuadTexCoord(gl_Position);
    vScreenPos.y = 1.0 - vScreenPos.y;
}

void PS()
{
    gl_FragColor = texture2D(sDiffMap, vScreenPos);
}
