#shader vertex
layout(location = 0) in vec4 position;

out vec3 v_TexCoords;
uniform mat4 u_MVP;

void main()
{
	v_TexCoords = vec3(position);
	gl_Position = u_MVP * position;
}


#shader fragment
precision mediump float;
layout(location = 0) out vec4 color;

in vec3 v_TexCoords;
uniform samplerCube u_Texture;

void main()
{
	color = texture(u_Texture,v_TexCoords);
}