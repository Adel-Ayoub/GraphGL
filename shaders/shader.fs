#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in float heightY;

uniform vec3 color;
uniform bool use_line;
uniform bool use_gridline;
uniform bool use_heatmap;
uniform float min_height;
uniform float max_height;
uniform float point_opacity;

vec3 computeColor(float value)
{
    if (value <= 0.2) {
        return mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), value / 0.2);
    } else if (value <= 0.4) {
        return mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0), (value - 0.2) / 0.2);
    } else if (value <= 0.6) {
        return mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (value - 0.4) / 0.2);
    } else if (value <= 0.8) {
        return mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.5, 0.0), (value - 0.6) / 0.2);
    } else {
        return mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 0.0, 0.0), (value - 0.8) / 0.2);
    }
}

void main()
{
    if (use_line) {
        FragColor = vec4(color, 0.1);
    }
    else if (use_gridline) {
        FragColor = vec4(color, 1.0);
    }
    else if (use_heatmap) {
        // Guard against zero range to avoid NaN when all vertices share the same height.
        float range = max_height - min_height;
        float normalizedHeight = (range > 0.0) ? (heightY - min_height) / range : 0.5;
        normalizedHeight = clamp(normalizedHeight, 0.0, 1.0);
        vec3 heatmap_color = computeColor(normalizedHeight);

        FragColor = vec4(heatmap_color, point_opacity);
    }
    else {
        FragColor = vec4(ourColor, point_opacity);
    }
}
