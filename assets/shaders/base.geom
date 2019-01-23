#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_Pos[3];
in vec3 v_Norm[3];
out vec3 g_Pos;
out vec3 g_Norm;

uniform mat4 u_matP;

void main() {

    // Normal
    // vec4 BA = gl_in[1].gl_Position - gl_in[0].gl_Position;
    // vec4 CA = gl_in[2].gl_Position - gl_in[0].gl_Position;
    // vec3 N = normalize(cross(-BA.xyz, CA.xyz));

    // "triangle_strip" primitive
    for (uint i = 0; i < 3; ++i) {
        g_Pos = v_Pos[i];
        g_Norm = v_Norm[i];
        gl_Position = u_matP * gl_in[i].gl_Position;
        EmitVertex();
    }

    // Close "triangle_strip" primitive
	EndPrimitive();
}
