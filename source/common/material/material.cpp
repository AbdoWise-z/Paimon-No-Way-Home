#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        shader->use();
        pipelineState.setup();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    void Material::copyTo(Material *m) const {
        m->shader = shader;
        m->pipelineState = pipelineState;
        m->transparent = transparent;
    }

    Material *Material::copy() {
        auto* material = new Material();
        copyTo(material);
        return material;
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        Material::setup();
        shader->set("tint",tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    TintedMaterial *TintedMaterial::copy() {
        auto* material = new TintedMaterial();
        material->tint = tint;
        copyTo(material);
        return material;
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold",alphaThreshold);
        glActiveTexture(GL_TEXTURE0);  //activate the texture no 0
        texture->bind();                      //bind our texture data to texture no 0
        if (sampler != nullptr)
            sampler->bind(0);       //bind our sample  to texture no 0
        shader->set("tex",0);   //set our Texture2D "tex" to use texture no 0
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    TexturedMaterial* TexturedMaterial::copy() {
       auto k = new TexturedMaterial();
       k->sampler = sampler;
       k->texture = texture;
       k->alphaThreshold = alphaThreshold;
       k->tint = tint;
       copyTo(k);
       return k;
    }


    void DefaultMaterial::setup() const {
        Material::setup();
        shader->set("material.tint" , this->tint);
        shader->set("material.emission" , emission);

        if (texture != nullptr){
            glActiveTexture(GL_TEXTURE0);
            texture->bind();
            if (sampler != nullptr){
                sampler->bind(0);
            }
            shader->set("material.hasTexture" , (GLint) 1);
            shader->set("material.tex",0);   //set our Texture2D "tex" to use texture no 0
        }else{
            shader->set("material.hasTexture" , (GLint) 0);
        }

        shader->set("isSkybox" , isSkybox ? (GLint) 1 : (GLint) 0);
        shader->set("material.ambientReflectivity" , ambientReflectivity);
        shader->set("material.diffuseReflectivity" , diffuseReflectivity);
        shader->set("material.specularReflectivity" , specularReflectivity);
        shader->set("material.specularIntensity" , specularIntensity);
    }

    void DefaultMaterial::deserialize(const nlohmann::json &data) {
        Material::deserialize(data);

        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));

        tint                 = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        isSkybox             = data.value("isSkybox" , false);
        ambientReflectivity  = data.value("specularReflectivity" , ambientReflectivity);
        diffuseReflectivity  = data.value("diffuseReflectivity" , diffuseReflectivity);
        specularReflectivity = data.value("specularReflectivity" , specularReflectivity);
        specularIntensity    = data.value("specularIntensity" , specularIntensity);
        emission             = data.value("emission" , emission);
        if (emission != 0){
            std::cout << "emission: " << emission << std::endl;
        }
    }

    DefaultMaterial *DefaultMaterial::copy() {
        auto k = new DefaultMaterial();
        copyTo(k);
        k->texture = texture;
        k->sampler = sampler;
        k->specularIntensity = specularIntensity;
        k->ambientReflectivity = ambientReflectivity;
        k->specularReflectivity = specularReflectivity;
        k->diffuseReflectivity = diffuseReflectivity;
        k->tint = tint;
        k->isSkybox = isSkybox;
        k->emission = emission;
        return k;
    }


    void MultiTexturedMaterial::setup() const {
        TintedMaterial::setup();

        for (GLint i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            textures[i]->bind();
            if (samplers[i] != nullptr)
                samplers[i]->bind(i);
            shader->set(std::string("tex_").append(std::to_string(i)),
                        i);
        }
    }

    void MultiTexturedMaterial::deserialize(const nlohmann::json &data) {
        TintedMaterial::deserialize(data);
        //fixme: maybe implement this once day ..
        throw "Not implemented";
    }

    MultiTexturedMaterial *MultiTexturedMaterial::copy() {
        //fixme: maybe implement this once day ..
        throw "Not implemented";
        return nullptr;
    }
}