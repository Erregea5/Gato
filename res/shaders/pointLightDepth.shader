#shader vertex
layout(location = 0) in vec3 position;

uniform mat4 u_Model;

void main()
{
	gl_Position = u_Model * vec4(position, 1.0);
}

#shader geometry
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 u_shadowMatrices[6];

out vec4 FragPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; 
        for (int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = u_shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#shader fragment
precision mediump float;
in vec4 FragPos;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main()
{
    float lightDistance = length(FragPos.xyz - u_lightPos);
    gl_FragDepth = lightDistance / u_farPlane;
}