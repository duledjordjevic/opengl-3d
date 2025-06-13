#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;

uniform mat4 uM; // Transformation matrix
uniform mat4 uV; // Camera (view) matrix
uniform mat4 uP; // Projection matrix

out vec4 channelCol;

void main()
{
	gl_Position = uP * uV * uM * vec4(inPos, 1.0); // Because matrix multiplication is non-commutative, we must multiply the MVP matrices and vertices in reverse order
	channelCol = inCol;
}
