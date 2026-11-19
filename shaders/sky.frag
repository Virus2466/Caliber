#version 460 core

in vec3 v_worldRay;
out vec4 FragColor;

uniform vec3 u_sunDirection;
uniform vec3 u_sunRadiance;
uniform float u_sunAngularRadius;
uniform float u_exposure;

void main() {
    vec3 ray = normalize(v_worldRay);
    float skyHeight = pow(max(ray.y, 0.0), 0.45);
    vec3 horizon = vec3(0.58, 0.69, 0.80);
    vec3 zenith = vec3(0.08, 0.23, 0.48);
    vec3 sky = mix(horizon, zenith, skyHeight);

    // A simple lower hemisphere until terrain is introduced.
    vec3 ground = vec3(0.035, 0.045, 0.055);
    vec3 color = mix(ground, sky, smoothstep(-0.025, 0.025, ray.y));

    float sunCosine = dot(ray, u_sunDirection);
    float edge = max(fwidth(sunCosine), 0.000001);
    float disc = smoothstep(cos(u_sunAngularRadius) - edge,
                           cos(u_sunAngularRadius) + edge, sunCosine);
    float halo = pow(max(sunCosine, 0.0), 64.0) * 0.15
               + pow(max(sunCosine, 0.0), 512.0) * 0.4;
    float aboveHorizon = smoothstep(0.0, 0.02, ray.y);
    color += u_sunRadiance * (disc * 12.0 + halo) * aboveHorizon;

    // Match the PBR shader: linear lighting, exposure, then display gamma.
    color = vec3(1.0) - exp(-color * u_exposure);
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}
