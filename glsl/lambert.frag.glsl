#version 150
// ^ Change this to version 130 if you have compatibility issues

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;

out vec4 out_Col;

void main()
{
    // Material base color (before shading)
    vec4 diffuseColor = fs_Col;

    // Calculate the diffuse term
    float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
    // Avoid negative lighting values
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm;

    // Compute final shaded color
    out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
}
