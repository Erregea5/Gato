#shader vertex
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_directLightMatrix;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_TexCoord;
out vec4 v_directShadowPos;

void main()
{
	vec4 pos = u_Model * vec4(position, 1.0);
	gl_Position = u_MVP * vec4(position, 1.0);
	v_position = vec3(pos);
	v_directShadowPos = u_directLightMatrix * pos;
	v_TexCoord = texCoord;
	v_normal = transpose(inverse(mat3(u_Model))) * normal;
}


#shader fragment
precision mediump float;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;

uniform vec3 u_directLightDirection;
uniform vec3 u_directLightColor;
uniform float u_directLightIntensity;
uniform sampler2D u_directLightShadowMap;
uniform int u_usingDirectional = 0;

uniform vec3 u_pointLightPosition;
uniform vec3 u_pointLightColor;
uniform float u_pointLightIntensity;
uniform samplerCube u_pointLightShadowMap;
uniform float u_farPlane;
uniform int u_usingPoint = 0;

uniform sampler2D u_Texture;
uniform int u_usingTexture = 0;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_TexCoord;
in vec4 v_directShadowPos;

float isInPointShadow() 
{
	vec3 diff = v_position - u_pointLightPosition;
	float depth=texture(u_pointLightShadowMap, diff).r*u_farPlane;
	float bias = max(.05 * dot(diff, -v_normal), .001);
	float shadow = depth < length(diff) - bias ? 1.0 : 0.0;
	return shadow;
}

float isInDirectShadow()
{
	vec3 shadowProjCoords = v_directShadowPos.xyz * .5 + .5;
	if (shadowProjCoords.z > 1.0)
		return 0.0;
	float bias = max(.05 * dot(u_directLightDirection, v_normal), .001);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_directLightShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_directLightShadowMap, shadowProjCoords.xy + vec2(x, y) * texelSize).r;
			shadow += shadowProjCoords.z - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

void main()
{
	float ambientLight = .2;

	vec3 pointLightColor;
	if (u_usingPoint==1)
	{
		vec3 direction = u_pointLightPosition - v_position;
		float diff = max(dot(normalize(direction), normalize(v_normal)),0.0f);
		float distance = length(direction);
		pointLightColor = (1 - isInPointShadow()) * u_pointLightColor * u_pointLightIntensity * diff / distance;
	}
	else pointLightColor = vec3(0.0);
	

	vec3 directLightColor;
	if (u_usingDirectional == 1)
	{
		float directDiff = max(dot(normalize(u_directLightDirection), normalize(v_normal)), 0.0f);
		directLightColor = (1.0 - isInDirectShadow()) * u_directLightColor * u_directLightIntensity * directDiff;
	}
	else directLightColor = vec3(0.0);

	vec4 texColor;
	if (u_usingTexture == 1)
		texColor = texture(u_Texture, v_TexCoord);
	else
		texColor = vec4(1.0);


	vec3 sum = pointLightColor + directLightColor + ambientLight;
	color = texColor * vec4(sum, 1.0);
}