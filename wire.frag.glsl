#version 150
// ^ Change this to version 130 if you have compatibility issues

in vec4 fs_Color;

out vec4 out_Color;

void main()
{
    // Copy the color; there is no shading.
    out_Color = fs_Color;
}
