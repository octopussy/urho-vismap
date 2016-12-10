#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"

uniform vec2 cCenterPos;
uniform float cVisMapShift;

void VS()
{

    mat4 modelMatrix = iModelMatrix;
        vec3 worldPos = GetWorldPos(modelMatrix);

        vec2 dir = normalize(vec2(worldPos) - cCenterPos);
        worldPos.x = worldPos.x + dir.x * cVisMapShift;
        worldPos.y = worldPos.y + dir.y * cVisMapShift;
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
