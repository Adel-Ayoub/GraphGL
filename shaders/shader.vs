#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

out vec3 ourColor;
out float heightY;

void main()
{
    ourColor = aColor;
    gl_PointSize = point_size;
    vec4 worldPos = model * vec4(aPos, 1.0);
    heightY = worldPos.y;
    gl_Position = projection * view * worldPos;
    
}
