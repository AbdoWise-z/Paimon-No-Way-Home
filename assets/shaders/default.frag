#version 330 core
#define MAX_LIGHTS 20

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 position;
} fs_in;

out vec4 frag_color;

//material
uniform struct Material {
    vec4 tint;
    int hasTexture; //1 == true , 0 == false
    sampler2D tex;
    vec3 ambientReflectivity;
    vec3 diffuseReflectivity;
    vec3 specularReflectivity;
    float specularIntensity;
} material;


//lighting
uniform int spotLightsCount = 0;
uniform struct SpotLight {
    vec3 position;
    float intensity;
    vec3 color;
    int decay;
} spotLights [MAX_LIGHTS];

uniform int directionalLightCount = 0;
uniform struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;
} directionalLights [MAX_LIGHTS];

uniform int coneLightsCount = 0;
uniform struct ConeLight {
    vec3 position;
    float intensity;
    vec3 color;
    vec3 direction;
    vec2 range;
    int smoothing; // 0 = disable , 1 = max , 2 = smooth step from low to high
    int decay;
} coneLights [MAX_LIGHTS];

uniform int isSkybox = 0; //sky boxes are not affected by normals or spot lights when renderered
uniform vec3 areaLight = vec3(1,1,1);
uniform vec3 cameraPosition = vec3(15,15,15);

void main(){
    //calculate the base color
    vec4 baseColor = material.tint * fs_in.color;
    if (material.hasTexture == 1){
        baseColor = baseColor * texture(material.tex, fs_in.tex_coord);
    }

    if (isSkybox == 1){ // no need to do light calculations
        frag_color = baseColor * vec4(areaLight * material.ambientReflectivity, 1.0);
        return;
    }

    vec3 fNormal = normalize(fs_in.normal);
    vec3 point2Cam = normalize(cameraPosition - fs_in.position);

    vec3 specularLight = vec3(0,0,0);
    //calculate the total directional light
    vec3 directionalLight = vec3(0,0,0);
    for (int i = 0;i < directionalLightCount;i++){
        directionalLight +=
              max( dot(-fNormal , normalize(directionalLights[i].direction)) , 0)
            * directionalLights[i].color
            * directionalLights[i].intensity;

        vec3 ref = reflect(directionalLights[i].direction , fNormal);
        float sF = max(dot(ref , point2Cam) , 0.0);
        sF = pow(sF , material.specularIntensity);
        specularLight += sF * directionalLights[i].color * directionalLights[i].intensity;
    }

    //calculate the total spot light
    vec3 spotLight = vec3(0,0,0);
    for (int i = 0;i < spotLightsCount;i++){
        vec3 diff = fs_in.position - spotLights[i].position;
        vec3 ndiff = normalize(diff);
        float divider = dot(diff, diff);
        if (spotLights[i].decay == 1){
            divider = sqrt(divider);
        }

        spotLight += max(dot(-fNormal, ndiff), 0) * spotLights[i].color * spotLights[i].intensity / divider;

        vec3 ref = reflect(ndiff , fNormal);
        float sF = max(dot(ref , point2Cam) , 0.0);
        sF = pow(sF , material.specularIntensity);
        specularLight += sF * spotLights[i].color * spotLights[i].intensity / divider;
    }

    //calculate the total cone light
    vec3 coneLight = vec3(0,0,0);
    for (int i = 0;i < coneLightsCount;i++){
        vec3 diff = fs_in.position - coneLights[i].position;
        vec3 ndiff = normalize(diff);
        float div = dot(ndiff , normalize(coneLights[i].direction));
        float divider = dot(diff, diff);
        if (coneLights[i].decay == 1){
            divider = sqrt(divider);
        }

        if (div >= coneLights[i].range.x && div <= coneLights[i].range.y){
            div = coneLights[i].smoothing == 1 ? 1 : div;
            if (coneLights[i].smoothing == 2){
                div = smoothstep(coneLights[i].range.x , coneLights[i].range.y , div);
            }
            coneLight += max( dot(-fNormal, ndiff) , 0) * coneLights[i].color * coneLights[i].intensity / divider * div;

            vec3 ref = reflect(ndiff , fNormal);
            float sF = max(dot(ref , point2Cam) , 0.0);
            sF = pow(sF , material.specularIntensity);
            specularLight += sF * coneLights[i].color * coneLights[i].intensity / divider * div;
        }
    }

    vec3 totalLight = (specularLight * material.specularReflectivity) + ((directionalLight + spotLight + coneLight) * material.diffuseReflectivity) + (areaLight * material.ambientReflectivity);
    frag_color = baseColor * vec4(totalLight , 1.0);
}