#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 clipSpace;
in vec3 toCamera;
in vec3 fromLight;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float waveSpeed;

uniform vec3 lightColor;


void main()
{
   vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
   vec2 reflectCords = vec2(ndc.x, - ndc.y);
   vec2 refractCords = vec2(ndc.x, ndc.y);
   
   vec2 distortion = (texture(dudvMap,vec2(TexCoords.x + waveSpeed,TexCoords.y)).rg ) * 1;
   distortion = TexCoords + vec2(distortion.x,distortion.y + waveSpeed);
   vec2 distortionEnd = (texture(dudvMap,distortion).rg * 2.0 -1.0) * 0.02;
   
   reflectCords +=distortionEnd;
   reflectCords.x = clamp(reflectCords.x,0.001,0.999);
   reflectCords.y = clamp(reflectCords.y, -0.999,-0.001);
   refractCords +=distortionEnd;
   refractCords = clamp(refractCords,0.001,0.999);
   
   vec4 reflectColor = texture(reflectionTexture,reflectCords);
   vec4 refractColor = texture(refractionTexture,refractCords);
   
   vec3 viewVector = normalize(toCamera);
   float refractiveFactor = dot(viewVector,vec3(0,1,0));
   refractiveFactor = pow(refractiveFactor,1);
   
   vec4 normalMapColor = texture(normalMap,distortion);
   vec3 normal = vec3(normalMapColor.r * 2.0 -1.0,normalMapColor.b,normalMapColor.g * 2.0 -1.0);
   normal = normalize(normal);
   
   vec3 reflectedLight = reflect(normalize(fromLight),normal);
   float spec = pow(max(dot(reflectedLight, viewVector), 0.0f), 2);
   vec3 speculaHighlights = lightColor * spec * 0.2;
   
   FragColor = mix(reflectColor,refractColor, refractiveFactor);
   FragColor = mix(FragColor,vec4(0.2,0.6,1.0,1) + vec4(speculaHighlights,0.0),0.5);
}