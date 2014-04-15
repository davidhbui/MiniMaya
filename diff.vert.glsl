#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;
uniform mat4 u_ModelInvTranspose;
uniform mat4 u_ViewProj;

in vec4 vs_Normal;
in vec4 vs_Position;
in vec4 vs_Color;

out vec4 fs_Normal;
out vec4 fs_LightVector;
out vec4 fs_Color;

const vec4 lightPos = vec4(5, 5, 3, 1);

void main()
{
    fs_Color = vs_Color;
    fs_Normal = u_ModelInvTranspose * vs_Normal;

    vec4 modelposition = u_Model * vs_Position;

    // Set up our vector for the light
    fs_LightVector = lightPos - modelposition;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * u_Model * vs_Position;
}
