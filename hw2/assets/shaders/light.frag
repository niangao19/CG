#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

void main() {
    vec3 light = vec3(0.0);
    vec3 ambient = material.ambient;
    vec3 diffuse = material.diffuse;
    vec3 specular = material.specular;
    float shininess = material.shininess;
    vec3 viewDir = normalize(viewPos - FragPos);
    if (  dl.enable == 1 ) {
        vec3 dl_am = ambient;
        vec3 lightDir = normalize( -dl.direction );
        vec3 dl_di = diffuse * max( dot( Normal , lightDir )  , 0.0 );
        vec3 halfDir = 0.5 * ( lightDir + viewDir );
        vec3 dl_sp = specular * pow( max( dot( Normal , halfDir )  , 0.0 )  , shininess  );
        light+= dl.lightColor*( dl_am + dl_di + dl_sp );
    } // if

    if (  pl.enable == 1 ) {
        vec3 pl_am = ambient;
        vec3 lightDir = normalize( pl.position - FragPos );
        vec3 pl_di = diffuse * max( dot( Normal , lightDir )  , 0.0 );
        vec3 halfDir = 0.5 * ( lightDir + viewDir );
        vec3 pl_sp = specular * pow( max( dot( Normal , halfDir )  , 0.0 )  , shininess  );
        float dist = length( pl.position - FragPos );
        float attenuation =  1.0 / (pl.constant + pl.linear * dist + pl.quadratic * (dist * dist));
        light += pl.lightColor*attenuation*( pl_am + pl_di + pl_sp );
    } // if


    if (  sl.enable == 1 ) {
        vec3 sl_am = ambient;
        vec3 lightDir = normalize( sl.position - FragPos );
        vec3 halfDir = 0.5 * ( lightDir + viewDir );
        float theta = dot(lightDir, normalize(-sl.direction));
        vec3 sl_di, sl_sp;
        float attenuation = 0;
        float dist = length( sl.position - FragPos );
        if(  theta >= sl.cutOff ) {
             sl_di = diffuse * max( dot( Normal , lightDir )  , 0.0 );
             halfDir = 0.5 * ( lightDir + viewDir );
             sl_sp = specular * pow( max( dot( Normal , halfDir )  , 0.0 )  , shininess  );
                    
        }
        else {
            sl_di = vec3(0,0,0);
            sl_sp = vec3(0,0,0);
        }
        attenuation =  1.0 / (pl.constant + pl.linear * dist + pl.quadratic * (dist * dist)); 
        light += sl.lightColor*attenuation*( sl_am + sl_di + sl_sp );
    } // if

    color = vec4(light, 1.0)*texture(ourTexture, TexCoord);
}
