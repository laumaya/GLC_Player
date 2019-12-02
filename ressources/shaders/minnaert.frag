
varying vec3 normal;
varying vec3 vpos;

uniform sampler2D tex;
uniform bool useTexture;

void main (void)
{
   vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);
   vec3 E = normalize(-vpos); // we are in Eye Coordinates, so EyePos is (0,0,0)
   vec3 R = normalize(-reflect(L,normal));
   vec3 RBack = normalize(-reflect(L,-normal));
   //calculate Ambient Term:
   vec4 Iamb = gl_FrontLightProduct[0].ambient;
   vec4 IambBack= gl_BackLightProduct[0].ambient;

   //calculate Diffuse Term:

    vec4 Idiff;
    if (useTexture)
    {
        Idiff = texture2D(tex, gl_TexCoord[0].xy) * max(dot(normal,L), 0.0) * gl_LightSource[0].diffuse;
        Idiff = clamp(Idiff, 0.0, 1.0);
    }
    else
    {
        Idiff = gl_FrontLightProduct[0].diffuse * max(dot(normal,L), 0.0);
        Idiff = clamp(Idiff, 0.0, 1.0);
    }

    vec4 IdiffBack;
    if (useTexture)
    {
        IdiffBack = texture2D(tex, gl_TexCoord[0].xy) * max(dot(-normal,L), 0.0) * gl_LightSource[0].diffuse;
        IdiffBack = clamp(IdiffBack, 0.0, 1.0);
    }
    else
    {
        IdiffBack = gl_BackLightProduct[0].diffuse * max(dot(-normal,L), 0.0);
        IdiffBack = clamp(IdiffBack, 0.0, 1.0);
    }

   // calculate Specular Term:
   vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
   Ispec = clamp(Ispec, 0.0, 1.0);

   vec4 IspecBack = gl_BackLightProduct[0].specular * pow(max(dot(RBack,E),0.0),0.3*gl_FrontMaterial.shininess);
   IspecBack = clamp(IspecBack, 0.0, 1.0);

   // write Total Color:
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;
   gl_FragColor += gl_BackLightModelProduct.sceneColor + IambBack + IdiffBack + IspecBack;
   gl_FragColor.a= gl_FrontMaterial.diffuse.a;
}
