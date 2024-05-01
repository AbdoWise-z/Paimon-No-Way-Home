#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "../../globals.h"


namespace our
{
    class ShaderProgram
    {

    private:
        // Shader Program Handle (OpenGL object name)
        GLuint program;

    public:
        ShaderProgram()
        {
            // TODO: (Req 1) Create A shader program
            program = glCreateProgram();
        }
        ~ShaderProgram()
        {
            // TODO: (Req 1) Delete a shader program
            if (program != 0)
                glDeleteProgram(program);
        }

        [[nodiscard]] bool attach(const std::string &filename, GLenum type) const;

        [[nodiscard]] bool link() const;

        void use() const
        {
            glUseProgram(program);
        }

        [[nodiscard]] GLint getUniformLocation(const std::string &name) const
        {
            // TODO: (Req 1) Return the location of the uniform with the given name
            return glGetUniformLocation(program, name.c_str());
        }

        void set(const std::string &uniform, GLfloat value) const
        {
            // TODO: (Req 1) Send the given float value to the given uniform
            GLint loc = getUniformLocation(uniform);
            if (loc == -1 && !SUPPRESS_SHADER_ERRORS){
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform1f(loc, value);
        }

        void set(const std::string &uniform, GLuint value) const
        {
            // TODO: (Req 1) Send the given unsigned integer value to the given uniform
            // Get the location of the uniform
            GLint location = getUniformLocation(uniform);

            // Check if the uniform exists
            if (location == -1 && !SUPPRESS_SHADER_ERRORS)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            // Send the value to the uniform
            glUniform1ui(location, value);
        }

        void set(const std::string &uniform, GLint value) const
        {
            // TODO: (Req 1) Send the given integer value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1 && !SUPPRESS_SHADER_ERRORS)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform1i(location, value);
        }

        void set(const std::string &uniform, glm::vec2 value) const
        {
            // TODO: (Req 1) Send the given 2D vector value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1 && !SUPPRESS_SHADER_ERRORS)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform2f(location, (float) value.x, (float) value.y);
        }

        void set(const std::string &uniform, glm::vec3 value) const
        {
            // TODO: (Req 1) Send the given 3D vector value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1 && !SUPPRESS_SHADER_ERRORS)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform3f(location, value.x, value.y, value.z);
        }

        void set(const std::string &uniform, glm::vec4 value) const
        {
            // TODO: (Req 1) Send the given 4D vector value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1 && !SUPPRESS_SHADER_ERRORS)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform4f(location, value.x, value.y, value.z, value.w);
        }

        void set(const std::string &uniform, glm::mat4 matrix) const
        {
            // TODO: (Req 1) Send the given matrix 4x4 value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1 && !SUPPRESS_SHADER_ERRORS)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        }

        // TODO: (Req 1) Delete the copy constructor and assignment operator.
        // Question: Why do we delete the copy constructor and assignment operator?
        /*
         * Unique Ownership: If the class owns some resources (e.g., a file handle, a network connection, or a GPU resource),
         * you typically want to ensure that only one instance of the class can own and manage those resources at a time.
         * Deleting the copy constructor and assignment operator prevents the creation of copies,
         * which could lead to resource leaks or other undefined behavior.
         * */
        ShaderProgram(const ShaderProgram &) = delete;            // Delete the copy constructor
        ShaderProgram &operator=(const ShaderProgram &) = delete; // Delete the assignment operator
    };

}

#endif