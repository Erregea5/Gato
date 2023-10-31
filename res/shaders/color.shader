#shader vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorCoord;

uniform mat4 u_MVP;
out vec3 v_colorCoord;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
	v_colorCoord = colorCoord;
}


#shader fragment
precision mediump float;

in vec3 v_colorCoord;

void main()
{
	gl_FragColor = vec4(v_colorCoord,1.0);
}