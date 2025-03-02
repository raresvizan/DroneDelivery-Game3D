#version 330

in vec2 texCoord; 
out vec4 fragmentColor;

uniform vec3 greenColor; 
uniform vec3 brownColor; 
uniform float noiseFrequency; 


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
    float terrainNoise = noise(texCoord * noiseFrequency);
    vec3 interpolatedColor = mix(greenColor, brownColor, terrainNoise);
    fragmentColor = vec4(interpolatedColor, 1.0);
}
