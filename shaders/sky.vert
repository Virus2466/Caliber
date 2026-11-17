#version 460 core

uniform mat4 u_inverseProjection;
uniform mat4 u_inverseViewRotation;

out vec3 v_worldRay;

void main() {
    // A single oversized triangle covers the viewport without a diagonal seam.
    vec2 position = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;
    vec4 viewRay = u_inverseProjection * vec4(position, 1.0, 1.0);
    v_worldRay = mat3(u_inverseViewRotation) * viewRay.xyz;
    gl_Position = vec4(position, 1.0, 1.0);
}
