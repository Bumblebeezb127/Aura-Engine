#type vertex
#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main(){

	v_TexCoord = TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
}



#type fragment
#version 430

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main(){
	color = texture(u_Texture, v_TexCoord * 10.0) * u_Color;
}
