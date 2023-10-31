#shader vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(position,1.0);//vec4(texCoord.xy * 2.0 - 1.0, 0.0, 1.0);

	v_TexCoord = texCoord;
}



#shader fragment
precision mediump float;
layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;
in vec2 v_TexCoord;

float sumOfSins(float y)
{
	float sum=0;
	for (int i = 0;i < 5;i++)
	{
		float amp = .08*y/sqrt(i+1);
		float freq = 2*exp(i)/(.2+y);
		float off = 1.5*sin(y*i*4);
		sum += amp * sin(freq * y + off);
	}
	return sum;
}

int quadrant(float x, float y)
{
	if (x < .5)
	{
		if (y < .5)
			return 3;
		return 2;
	}
	if (y < .5)
		return 4;
	return 1;
}

bool isInTree(float x, float y)
{
	int corner = quadrant(x, y);
	if (corner == 2)
		return y < 1.0 - x+sumOfSins(y);
	return abs(x - .5) < .05 + sumOfSins(y);//.01*sin((y+sin(x+y)) * 100);
}

void main()
{
	color = vec4(1.0,1.0,0.0,1.0);
	return;
	float x = v_TexCoord.x;
	float y = v_TexCoord.y;

	vec3 col = vec3(x, y, 1.0);
	if (isInTree(x, y))
		col = vec3(0.0, 1.0, 1.0);
	color = vec4(col, 1.0);
}

