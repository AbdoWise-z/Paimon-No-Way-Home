#include <sstream>
#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include "../deserialize-utils.hpp"

namespace our {

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json& config){
        // First, we store the window size for later use
        this->windowSize = windowSize;
        this->areaLight = config.value("areaLight" , glm::vec3(1,1,1));
        // Then we check if there is a sky texture in the configuration
        if(config.contains("sky")){
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));
            
            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram* skyShader = new ShaderProgram();
            //Fixme: change to texture if needed
            skyShader->attach("assets/shaders/default.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/default.frag", GL_FRAGMENT_SHADER);
            skyShader->link();
            
            //TODO: (Req 10) Pick the correct pipeline state to draw the sky
            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.frontFace = GL_CW;
            skyPipelineState.depthMask = true;
            skyPipelineState.depthTesting.enabled = true;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            auto skyTextureFile = config.value<std::string>("sky", "");
            Texture2D* skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            auto* skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterial = new DefaultMaterial(); //Fixme change back to Textured Material if needed
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->transparent = false;
            this->skyMaterial->isSkybox = true;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if(config.contains("postprocess")){
            //TODO: (Req 11) Create a framebuffer
            const auto& postprocess = config["postprocess"];
            int tex_count = postprocess.value("channels" , 1);

            postprocessFramebuffer = new Framebuffer(windowSize);
            postprocessFramebuffer->bind();
            for (int i = 0;i < tex_count;i++)
                postprocessFramebuffer->addColorTexture(GL_RGBA8);
            postprocessFramebuffer->addDepthTexture(GL_DEPTH_COMPONENT24);
            postprocessFramebuffer->unbind();

            postprocessFramebuffer2 = new Framebuffer(windowSize);
            postprocessFramebuffer2->bind();
            for (int i = 0;i < tex_count;i++)
                postprocessFramebuffer2->addColorTexture(GL_RGBA8);
            postprocessFramebuffer2->addDepthTexture(GL_DEPTH_COMPONENT24);
            postprocessFramebuffer2->unbind();

            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post-processing shader
            auto effects = postprocess["effects"];
            for (const auto& effect : effects) {
                auto *postprocessShader = new ShaderProgram();
                postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
                postprocessShader->attach(effect.value<std::string>("target", ""), GL_FRAGMENT_SHADER);
                postprocessShader->link();
                postprocessShaders.emplace_back(postprocessShader);
                postprocessData.emplace_back(effect["params"]);
                std::cout << "Generated Postprocess Shader: " << effect.value<std::string>("target", "") << std::endl;
            }

            // Create a post processing material
            postprocessMaterial = new MultiTexturedMaterial();
            postprocessMaterial->shader = nullptr;
            postprocessMaterial->pipelineState.depthMask = false;

        }
    }

    void ForwardRenderer::destroy(){
        // Delete all objects related to the sky
        if(skyMaterial){
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if(postprocessMaterial){
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete postprocessFramebuffer;
            delete postprocessFramebuffer2;
            delete postprocessMaterial->samplers[0];
            delete postprocessMaterial;

            for (auto k : postprocessShaders){
                delete k;
            }

            postprocessShaders.clear();
            postprocessData.clear();
        }
    }

    void ForwardRenderer::render(World* world){
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent* camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        directionalLights.clear();
        spotLights.clear();
        coneLights.clear();

        for(auto entity : world->getEntities()){
            // If we hadn't found a camera yet, we look for a camera in this entity
            if(!camera) camera = entity->getComponent<CameraComponent>();

            glm::mat4 localToWorld = entity->getLocalToWorldMatrix();
            glm::vec4 position = localToWorld * glm::vec4(0, 0, 0, 1);

            // If this entity has a mesh renderer component
            if(auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer){
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = localToWorld;
                command.center = glm::vec3(position);
                command.mesh = meshRenderer->mesh;
                command.shapeID = meshRenderer->shapeID;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if(command.material->transparent){
                    transparentCommands.push_back(command);
                } else {
                // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }

            auto dl = entity->getComponent<DirectionalLight>();
            if (dl != nullptr)
                directionalLights.emplace_back(dl);

            auto sl = entity->getComponent<SpotLight>();
            if (sl != nullptr) {
                spotLights.emplace_back(sl);
                sl->worldPosition = glm::vec3(position);
            }

            auto cl = entity->getAllComponents<ConeLight>();
            for (auto k : cl){
                coneLights.emplace_back(k);
                k->worldPosition = glm::vec3(position);
                k->worldDirection = glm::vec3(localToWorld * glm::vec4(k->direction , 0.0));
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if(camera == nullptr) return;

        //TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        auto camTransform = camera->getOwner()->getLocalToWorldMatrix();
        glm::vec4 cameraForward_ =  camTransform * glm::vec4(0.0, 0.0, -1.0f , 0.0);
        glm::vec4 cameraCenter_  =  camTransform * glm::vec4(0.0, 0.0,  0.0f , 1.0);

        glm::vec3 cameraForward = glm::vec3(cameraForward_.x , cameraForward_.y , cameraForward_.z);
        glm::vec3 cameraCenter  = glm::vec3(cameraCenter_.x  , cameraCenter_.y  , cameraCenter_.z );

        std::sort(
                transparentCommands.begin(),
                transparentCommands.end(),
                [cameraForward,cameraCenter](const RenderCommand& first, const RenderCommand& second){
            //TODO: (Req 9) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second".
            return glm::dot((second.center - cameraCenter) , cameraForward) <  glm::dot((first.center - cameraCenter) , cameraForward);
        });

        //TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        auto VP = camera->getProjectionMatrix(this->windowSize) * camera->getViewMatrix();

        //TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        glViewport(0,0,windowSize.x , windowSize.y);

        //TODO: (Req 9) Set the clear color to black and the clear depth to 1
        glClearColor(0,0,0,0);
        glClearDepth(1);

        //TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(true , true , true , true);
        glDepthMask(true);


        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial){
            //TODO: (Req 11) bind the framebuffer
            //glBindFramebuffer(GL_FRAMEBUFFER,this->postprocessFrameBuffer);
            postprocessFramebuffer->bind();
            auto* buff = new unsigned int[postprocessFramebuffer->getColorTexturesCount()];
            for (int i = 0;i < postprocessFramebuffer->getColorTexturesCount();i++){
                buff[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glDrawBuffers(postprocessFramebuffer->getColorTexturesCount() , buff);
            delete[] buff;
        }

        //TODO: (Req 9) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //TODO: (Req 9) Draw all the opaque commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto k : opaqueCommands){
            k.material->setup();
            if (dynamic_cast<DefaultMaterial*>(k.material)){

                // set up transform
                k.material->shader->set("transform", k.localToWorld);
                k.material->shader->set("Camera", VP);
                k.material->shader->set("cameraPosition", cameraCenter);
                k.material->shader->set("areaLight" , areaLight);

                // set up lights
                k.material->shader->set("directionalLightCount" , (GLint) directionalLights.size());
                for (int i = 0;i < directionalLights.size();i++){
                    std::stringstream ss;
                    ss << "directionalLights[" << i << "].";
                    auto header = ss.str();
                    k.material->shader->set( header + "direction" , directionalLights[i]->direction);
                    k.material->shader->set( header + "intensity" , directionalLights[i]->intensity);
                    k.material->shader->set( header + "ambientColor" , directionalLights[i]->ambientColor);
                    k.material->shader->set( header + "diffuseColor" , directionalLights[i]->diffuseColor);
                    k.material->shader->set( header + "specularColor" , directionalLights[i]->specularColor);
                }

                k.material->shader->set("spotLightsCount" , (GLint) spotLights.size());
                for (int i = 0;i < spotLights.size();i++){
                    std::stringstream ss;
                    ss << "spotLights[" << i << "].";
                    auto header = ss.str();
                    k.material->shader->set( header + "position" , spotLights[i]->worldPosition);
                    k.material->shader->set( header + "intensity" , spotLights[i]->intensity);
                    k.material->shader->set( header + "specularColor" , spotLights[i]->specularColor);
                    k.material->shader->set( header + "diffuseColor" , spotLights[i]->diffuseColor);
                    k.material->shader->set( header + "ambientColor" , spotLights[i]->ambientColor);
                    k.material->shader->set( header + "attenuation" , spotLights[i]->attenuation);
                }

                k.material->shader->set("coneLightsCount" , (GLint) coneLights.size());
                for (int i = 0;i < coneLights.size();i++){
                    std::stringstream ss;
                    ss << "coneLights[" << i << "].";
                    auto header = ss.str();
                    k.material->shader->set( header + "position" , coneLights[i]->worldPosition);
                    k.material->shader->set( header + "direction" , coneLights[i]->worldDirection);
                    k.material->shader->set( header + "intensity" , coneLights[i]->intensity);
                    k.material->shader->set( header + "range" , coneLights[i]->range);
                    k.material->shader->set( header + "ambientColor" , coneLights[i]->ambientColor);
                    k.material->shader->set( header + "specularColor" , coneLights[i]->specularColor);
                    k.material->shader->set( header + "diffuseColor" , coneLights[i]->diffuseColor);
                    k.material->shader->set( header + "attenuation" , coneLights[i]->attenuation);
                    k.material->shader->set( header + "smoothing" , coneLights[i]->smoothing);
                    k.material->shader->set( header + "range" , coneLights[i]->range);
                }
            }else{
                k.material->shader->set("transform", VP * k.localToWorld);
            }
            k.mesh->draw(k.shapeID);
        }

        // If there is a sky material, draw the sky
        if(this->skyMaterial){
            //TODO: (Req 10) setup the sky material
            skyMaterial->setup();
            skyMaterial->shader->set("areaLight" , areaLight);

            //TODO: (Req 10) Get the camera position
            //...

            //TODO: (Req 10) Create a model matrix for the sy such that it always follows the camera (sky sphere center = camera position)
            auto M = glm::translate(glm::mat4(1.0f) , cameraCenter);

            //TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            // We can achieve the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f
            ); //this thing gets transposed ...

            // Create a scale matrix for the skybox
            glm::mat4 skyboxScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera->orthoHeight * 2, camera->orthoHeight * 2, camera->orthoHeight * 2));

            //TODO: (Req 10) set the "transform" uniform
            skyMaterial->shader->set("transform", M * skyboxScaleMatrix);
            skyMaterial->shader->set("Camera", alwaysBehindTransform * VP);

            //TODO: (Req 10) draw the sky sphere
            skySphere->draw();
        }
        //TODO: (Req 9) Draw all the transparent commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto k : transparentCommands){
            k.material->setup();
            if (dynamic_cast<DefaultMaterial*>(k.material)){
                // set up transform
                k.material->shader->set("transform", k.localToWorld);
                k.material->shader->set("Camera", VP);
                k.material->shader->set("cameraPosition", cameraCenter);
                k.material->shader->set("areaLight" , areaLight);

                // set up lights
                k.material->shader->set("directionalLightCount" , (GLint) directionalLights.size());
                for (int i = 0;i < directionalLights.size();i++){
                    std::stringstream ss;
                    ss << "directionalLights[" << i << "].";
                    auto header = ss.str();
                    k.material->shader->set( header + "direction" , directionalLights[i]->direction);
                    k.material->shader->set( header + "intensity" , directionalLights[i]->intensity);
                    k.material->shader->set( header + "ambientColor" , directionalLights[i]->ambientColor);
                    k.material->shader->set( header + "diffuseColor" , directionalLights[i]->diffuseColor);
                    k.material->shader->set( header + "specularColor" , directionalLights[i]->specularColor);
                }

                k.material->shader->set("spotLightsCount" , (GLint) spotLights.size());
                for (int i = 0;i < spotLights.size();i++){
                    std::stringstream ss;
                    ss << "spotLights[" << i << "].";
                    auto header = ss.str();
                    k.material->shader->set( header + "position" , spotLights[i]->worldPosition);
                    k.material->shader->set( header + "intensity" , spotLights[i]->intensity);
                    k.material->shader->set( header + "specularColor" , spotLights[i]->specularColor);
                    k.material->shader->set( header + "diffuseColor" , spotLights[i]->diffuseColor);
                    k.material->shader->set( header + "ambientColor" , spotLights[i]->ambientColor);
                    k.material->shader->set( header + "attenuation" , spotLights[i]->attenuation);
                }

                k.material->shader->set("coneLightsCount" , (GLint) coneLights.size());
                for (int i = 0;i < coneLights.size();i++){
                    std::stringstream ss;
                    ss << "coneLights[" << i << "].";
                    auto header = ss.str();
                    k.material->shader->set( header + "position" , coneLights[i]->worldPosition);
                    k.material->shader->set( header + "direction" , coneLights[i]->worldDirection);
                    k.material->shader->set( header + "intensity" , coneLights[i]->intensity);
                    k.material->shader->set( header + "range" , coneLights[i]->range);
                    k.material->shader->set( header + "ambientColor" , coneLights[i]->ambientColor);
                    k.material->shader->set( header + "specularColor" , coneLights[i]->specularColor);
                    k.material->shader->set( header + "diffuseColor" , coneLights[i]->diffuseColor);
                    k.material->shader->set( header + "attenuation" , coneLights[i]->attenuation);
                    k.material->shader->set( header + "smoothing" , coneLights[i]->smoothing);
                    k.material->shader->set( header + "range" , coneLights[i]->range);
                }
            }else{
                k.material->shader->set("transform", VP * k.localToWorld);
            }
            k.mesh->draw(k.shapeID);
        }

        // If there is a postprocess material, apply postprocessing
        if(postprocessMaterial){
            postprocessFramebuffer->unbind();


            our::SUPPRESS_SHADER_ERRORS = true; //for my mental stability ...
            glBindVertexArray(postProcessVertexArray);

            Framebuffer* from = postprocessFramebuffer ;
            Framebuffer* next = postprocessFramebuffer2;
            for (int i = 0;i < postprocessShaders.size();i++){
                auto bound = false;
                if (i != postprocessShaders.size() - 1) {
                    next->bind();
                    bound = true;
                    auto* buff = new unsigned int[next->getColorTexturesCount()];
                    for (int k = 0;k < next->getColorTexturesCount();k++){
                        buff[k] = GL_COLOR_ATTACHMENT0 + k;
                    }
                    glDrawBuffers(next->getColorTexturesCount() , buff);
                    delete[] buff;
                }

                auto shader = postprocessShaders[i];
                postprocessMaterial->textures.clear();
                postprocessMaterial->samplers.clear();
                for (int j = 0;j < from->getColorTexturesCount();j++){
                    postprocessMaterial->textures.emplace_back(from->getColorTexture(j));
                    postprocessMaterial->samplers.emplace_back(postprocessSampler);
                }

                postprocessMaterial->shader = shader;
                postprocessMaterial->setup();

                //now setup the extra params
                for (auto& [name, desc]  : postprocessData[i].items()){
                    if (desc.is_boolean()){
                        shader->set(name , desc.get<bool>() ? 1 : 0);
                    } else if (desc.is_number_float()){
                        shader->set(name , desc.get<float>());
                    } else if (desc.is_number_integer()){
                        shader->set(name , desc.get<int>());
                    } else if (desc.is_number()){
                        shader->set(name , desc.get<float>());
                    } else if (desc.is_array()){
                        int s = (int) desc.size();
                        if (s == 1){
                            shader->set(name , desc[0].get<float>());
                        } else if (s == 2) {
                            shader->set(name , desc.get<glm::vec2>());
                        } else if (s == 3) {
                            shader->set(name , desc.get<glm::vec3>());
                        } else if (s == 4) {
                            shader->set(name , desc.get<glm::vec4>());
                        }
                    }
                }

                glDrawArrays(GL_TRIANGLES,0,3);

                if (bound) next->unbind();

                if (next == postprocessFramebuffer){
                    next = postprocessFramebuffer2;
                    from = postprocessFramebuffer;
                }else {
                    next = postprocessFramebuffer;
                    from = postprocessFramebuffer2;
                }
            }
            our::SUPPRESS_SHADER_ERRORS = false;
        }
    }

}