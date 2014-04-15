#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Position;
in vec4 vs_Color;

out vec4 fs_Position;
out vec4 fs_Color;

void main()
{
    vec4 modelposition = u_Model * vs_Position;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * u_Model * vs_Position;

    fs_Color = vs_Color;
}
