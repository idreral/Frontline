#version 330
in vec2 fragTexCoord;
out vec4 finalColor;
uniform sampler2D texture0;
uniform float threshold;

void main() {
    vec3 color = texture(texture0, fragTexCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness < threshold)
        finalColor = vec4(0.0);
    else
        finalColor = vec4(color, 1.0);
}
