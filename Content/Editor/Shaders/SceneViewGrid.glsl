Culling = None

#begin vertex
#version 450

struct CameraData
{
	vec3 Position;
	float Near;
	vec3 ViewDirection;
	float Far;

	mat4 Projection;
	mat4 View;
	mat4 ViewProjection;

	mat4 InverseProjection;
	mat4 InverseView;
	mat4 InverseViewProjection;

	ivec2 ViewportSize;
	float FOV;
};

layout(std140, set = 0, binding = 0) uniform Camera
{
	CameraData u_Camera;
};

layout(location = 0) in vec2 i_Position;

layout(location = 0) out vec3 o_Position;
layout(location = 1) out vec3 o_CameraPosition;

layout(std140, push_constant) uniform Grid
{
	float CellSize;
	float Scale;
} u_Grid;

void main()
{
	vec3 worldSpacePosition = vec3(i_Position.x, 0.0, i_Position.y) * u_Grid.Scale;
	worldSpacePosition.xz += floor(u_Camera.Position.xz / u_Grid.CellSize) * u_Grid.CellSize;

	gl_Position = u_Camera.ViewProjection * vec4(worldSpacePosition, 1.0);

	o_Position = worldSpacePosition;
	o_CameraPosition = u_Camera.Position;
}
#end

#begin pixel
#version 450

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_CameraPosition;

layout(location = 0) out vec4 o_Color;

layout(std140, push_constant) uniform Grid
{
	layout(offset = 16) float FadeDistance;
	layout(offset = 32) vec3 Color;
} u_Grid;

void main()
{
	vec2 projectedPosition = i_Position.xz;
	vec2 projectedCameraPosition = i_CameraPosition.xz;

	float fade = min(1.0f, distance(projectedPosition, projectedCameraPosition) / u_Grid.FadeDistance);

	o_Color = vec4(u_Grid.Color, 1.0f - fade);
}

#end
