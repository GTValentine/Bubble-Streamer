#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;

in vec3 vs_Pos;
in vec3 vs_Nor;
in vec3 vs_Col;

out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec4 fs_Col;

const vec4 lightPos = vec4(5, 5, 3, 1);

void main()
{
    fs_Col = vec4(vs_Col, 1);
    fs_Nor = u_ModelInvTr * vec4(vs_Nor, 0);

    vec4 modelposition = u_Model * vec4(vs_Pos, 1);

    // Set up our vector for the light
    fs_LightVec = lightPos - modelposition;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;
}
