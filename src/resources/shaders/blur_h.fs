#version 330
in vec2 fragTexCoord;
out vec4 finalColor;
uniform sampler2D texture0;
uniform vec2 texSize;
uniform float radius = 5.0;

void main() {
    vec2 step = vec2(1.0 / texSize.x, 0.0);
    vec4 sum = vec4(0.0);
    for (float i = -radius; i <= radius; i++) {
        sum += texture(texture0, fragTexCoord + step * i);
    }
    finalColor = sum / (2.0 * radius + 1.0);
}
