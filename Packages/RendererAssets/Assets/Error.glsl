#begin vertex
#version 450

#include "Common/Camera.glsl"
#include "Common/Instancing.glsl"

layout(location = 0) in vec3 i_Position;

void main()
{
    gl_Position = u_Camera.ViewProjection * GetInstanceTransform() * vec4(i_Position, 1.0);
}
#end

#begin pixel
#version 450

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(1.0, 0.0, 1.0, 1.0);
}
#end
