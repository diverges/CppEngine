#version 330 core

// Vertex attributes
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

// Transformation matrices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// Output to fragment shader
out vec3 vertexColor;
out vec3 fragNormal;
out vec2 texCoord;
out vec3 fragPos;

void main() {
    // Transform position to clip space
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    gl_Position = uProjection * uView * worldPos;
    
    // Pass data to fragment shader
    vertexColor = aColor;
    fragNormal = mat3(transpose(inverse(uModel))) * aNormal;
    texCoord = aTexCoord;
    fragPos = worldPos.xyz;
}