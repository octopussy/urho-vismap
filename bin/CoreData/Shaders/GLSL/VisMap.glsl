#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"

uniform vec2 cCenterPos;

void VS()
{

    mat4 modelMatrix = iModelMatrix;
        vec3 worldPos = GetWorldPos(modelMatrix);

        vec2 dir = normalize(vec2(worldPos) - cCenterPos);
        worldPos.x = worldPos.x + dir.x * 10;
        worldPos.y = worldPos.y + dir.y * 10;
        worldPos.z = 0;

        gl_Position = GetClipPos(worldPos);
        gl_Position.z = 0;
       // vTexCoord = iPos.xyz;
}

void PS()
{
    vec4 diffColor = vec4(1, 0, 0, 1);

    gl_FragColor = diffColor;
}
