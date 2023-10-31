#shader vertex
layout(location = 0) in vec4 position;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * position;
}


#shader fragment
precision mediump float;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
	color = vec4(0.0,0.5,0.5,1.0);
}