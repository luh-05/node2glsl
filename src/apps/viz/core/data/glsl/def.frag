#version 450

layout(location = 0) in fragColor;
// layout(location = 1) in fragTextCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor.x, fragColor.y, fragColor.z, 1.0f);
}
