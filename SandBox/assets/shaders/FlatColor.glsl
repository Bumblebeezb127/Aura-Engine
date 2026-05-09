//Flat Color Shader

#type vertex
#version 430

layout(location = 0) in vec3 position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main(){
	gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
}



#type fragment
#version 430

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main(){
	color = u_Color;
}
