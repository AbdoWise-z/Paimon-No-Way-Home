#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
            if (program != NULL)
                glDeleteProgram(program)
        }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use()
        {
            glUseProgram(program);
        }

        GLuint getUniformLocation(const std::string &name)
        {
            // TODO: (Req 1) Return the location of the uniform with the given name
            return glGetUniformLocation(program, name.c_str());
        }

        void set(const std::string &uniform, GLfloat value)
        {
            // TODO: (Req 1) Send the given float value to the given uniform
            GLint loc = getUniformLocation(uniform);
            if (loc == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform1f(loc, value);
        }

        void set(const std::string &uniform, GLuint value)
        {
            // TODO: (Req 1) Send the given unsigned integer value to the given uniform
            // Get the location of the uniform
            GLint location = glGetUniformLocation(uniform);

            // Check if the uniform exists
            if (location == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            // Send the value to the uniform
            glUniform1ui(location, value);
        }

        void set(const std::string &uniform, GLint value)
        {
            // TODO: (Req 1) Send the given integer value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform1i(location, value);
        }

        void set(const std::string &uniform, glm::vec2 value)
        {
            // TODO: (Req 1) Send the given 2D vector value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform2f(location, value.x, value.y);
        }

        void set(const std::string &uniform, glm::vec3 value)
        {
            // TODO: (Req 1) Send the given 3D vector value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform3f(location, value.x, value.y, value.z)
        }

        void set(const std::string &uniform, glm::vec4 value)
        {
            // TODO: (Req 1) Send the given 4D vector value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniform4f(location, value.x, value.y, value.z, value.w);
        }

        void set(const std::string &uniform, glm::mat4 matrix)
        {
            // TODO: (Req 1) Send the given matrix 4x4 value to the given uniform
            GLint location = getUniformLocation(uniform);
            if (location == -1)
            {
                std::cerr << "Uniform '" << uniform << "' does not exist in the shader program." << std::endl;
                return;
            }
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        }

        // TODO: (Req 1) Delete the copy constructor and assignment operator.
        // Question: Why do we delete the copy constructor and assignment operator?
        /*Unique Ownership: If the class owns some resources (e.g., a file handle, a network connection, or a GPU resource),
        you typically want to ensure that only one instance of the class can own and manage those resources at a time.
        Deleting the copy constructor and assignment operator prevents the creation of copies,
         which could lead to resource leaks or other undefined behavior.*/
        ShaderProgram(const ShaderProgram &) = delete;            // Delete the copy constructor
        ShaderProgram &operator=(const ShaderProgram &) = delete; // Delete the assignment operator
    };

}

#endif