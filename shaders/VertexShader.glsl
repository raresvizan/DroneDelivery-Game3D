#version 330

layout(location = 0) in vec3 position;
out vec2 texCoord;

// uniformele pentru transformari
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float noiseFrequency;
uniform float amplitude;

float generateKeys(vec2 point) {
    return fract(sin(dot(point, vec2(197.4, 434.3))) * 564416.6846);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = generateKeys(i);
    float b = generateKeys(i + vec2(1.0, 0.0));
    float c = generateKeys(i + vec2(0.0, 1.0));
    float d = generateKeys(i + vec2(1.0, 1.0));

    vec2 u = f * f * f;

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}


void main() {
    vec3 adjustedPosition = position;
    
    float elevation = noise(adjustedPosition.xz * noiseFrequency) * amplitude;
    adjustedPosition.y += elevation;

    gl_Position = Projection * View * Model * vec4(adjustedPosition, 1.0);

    texCoord = position.xz;
}
