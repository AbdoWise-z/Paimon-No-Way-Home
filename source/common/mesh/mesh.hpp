#pragma once

#include <glad/gl.h>
#include "vertex.hpp"
#include "tinyobj/tiny_obj_loader.h"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount;
    public:

        std::vector<std::pair<unsigned int ,unsigned int>> shapes; //defines the start & end index of each shape
        std::vector<tinyobj::material_t> materials;

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: (Req 2) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            // position
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
            // color
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4,  GL_UNSIGNED_BYTE, true, sizeof(Vertex),  (void*)offsetof(Vertex, color));
            // texture
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(offsetof(Vertex, tex_coord)));
            // normal
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3,  GL_FLOAT, false, sizeof(Vertex),  (void*)offsetof(Vertex, normal));
            //element buffer
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof( unsigned int), elements.data(), GL_STATIC_DRAW);
            elementCount=(GLsizei) elements.size();

            // Unbind the Vertex array
            // To prevent  other meshes from Adding data to this VAO
            glBindVertexArray(0);

            // Unbind Vertex buffer and element buffer
            // it is optional because just binding a new buffer unbind previous one
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        }

        // this function should render the mesh
        void draw(int id = -1) const
        {
            //TODO: (Req 2) Write this function

            int count = elementCount;
            unsigned long long offset = 0;

            if (id != -1){
                auto shape = shapes[id];
                count = shape.second - shape.first + 1;
                offset = (unsigned long long) (shape.first * sizeof( unsigned int));
            }

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void *) offset);
            glBindVertexArray(0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 2) Write this function
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}