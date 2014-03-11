/* Copyright (c) Russell Gillette
 * December 2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * CPSC Computer Graphics 2014
 * University of British Columbia
 */

/* == MeshUtils.h ==
 * 
 * Utility and helper functions that operate on meshes or mesh data without a
 * specified Mesh instance.
 */

#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include "Mesh.h"
#include "OBJLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// load a Mesh object from an OBJ file.
Mesh *loadMeshFromFile(RenderState &state, string file_name)
{
    Mesh *m = NULL;

    // parse mesh
    ObjLoader obj_parser(file_name);
    obj_parser.generateNormals();

    // gather data from the parser to construct mesh
    GLubyte *data = NULL;
    int *indices;
    int data_size, num_indices, num_attr;
    obj_attrib_info *parsed_attr_info;
    attrib_info     *attr_info;

    // export parser data
    obj_parser.objExportGLSeparate(data_size, data, num_indices, indices, num_attr, parsed_attr_info);

    attr_info = new attrib_info[num_attr];
    for (int j = 0; j < num_attr; j++)
    {
        attr_info[j].attrib_number = parsed_attr_info[j].attrib_number;
        attr_info[j].attrib_size   = parsed_attr_info[j].attrib_size;
        attr_info[j].data_offset   = parsed_attr_info[j].data_offset;
        attr_info[j].data_stride   = parsed_attr_info[j].data_stride;
        attr_info[j].num_comp      = parsed_attr_info[j].num_comp;
    }

    m = new Mesh();
    m->init(1, state);
    m->loadVBuffer(0, data_size, data, 0, num_attr, attr_info);
    m->loadIBuffer(num_indices, sizeof(int), indices);

    delete [] attr_info;

    return m;
}

Mesh *createAxis(RenderState & state, float scale)
{
    Mesh *m;
    attrib_info details[2];

    // NOTE: vertices are duplicated, as theu have different normals
    // and this is not possible to represent in opengl
    float data[] = 
    {
        // vertices
         0.0f,  1.0f,  0.0f,  -0.04f, 0.0f,  0.04f,  0.04f, 0.0f,  0.04f,
         0.0f,  1.0f,  0.0f,  -0.04f, 0.0f, -0.04f, -0.04f, 0.0f,  0.04f,
         0.0f,  1.0f,  0.0f,   0.04f, 0.0f, -0.04f, -0.04f, 0.0f, -0.04f,
         0.0f,  1.0f,  0.0f,   0.04f, 0.0f,  0.04f,  0.04f, 0.0f, -0.04f,
         0.04f, 0.0f,  0.04f,  0.04f, 0.0f, -0.04f, -0.04f, 0.0f, -0.04f,
        -0.04f, 0.0f, -0.04f, -0.04f, 0.0f,  0.04f,  0.04f, 0.0f,  0.04f,
        // start normals: this is a hack to approximate reasonable normals
         0.0f,  0.0f,  1.0f,   0.0f, 0.0f,   1.0f,   0.0f,  0.0f,  1.0f,
        -1.0f,  0.0f,  0.0f,  -1.0f, 0.0f,   0.0f,  -1.0f,  0.0f,  0.0f,
         0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  -1.0f,   0.0f,  0.0f, -1.0f,
         1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f,   1.0f,  0.0f,  0.0f,
         0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f 
    };

    // scale the vertex locations relative to the scale
    // parameter (note I only go to 54, the num verts)
    for (int i = 0; i < 54; i++)
    {
        data[i] *= scale;
    }

    // since we have redefined our vertices, the indices
    // are just a linear ordering
    int indices[18] = {0,1,2,3,4,5,6,7,8,9,10,11,12,
        13,14,15,16,17};

    // attribute information for vertices
    details[0].attrib_number = 0;
    details[0].attrib_size   = 3 * sizeof(float); // size of one vertex
    details[0].data_offset   = 0; // byte offset to vertices
    details[0].data_stride   = 0; // space between vertices
    details[0].num_comp      = 3;

    // attribute information for normals
    details[1].attrib_number = 1;
    details[1].attrib_size   = 3 * sizeof(float); // size of one normal
    details[1].data_offset   = 18 * 3 * sizeof(float); // byte offset to normals
    details[1].data_stride   = 0; // space between normals
    details[1].num_comp      = 3;

    m = new Mesh();
    m->init(1, state);
    m->loadVBuffer(0, sizeof(data), (GLubyte*)data, 0, 2, details);
    m->loadIBuffer(18, sizeof(int), indices);

    return m;
}

Mesh *createGem(RenderState & state, float scale)
{
    Mesh *m;
    attrib_info details[2];

    // NOTE: vertices are duplicated, as theu have different normals
    // and this is not possible to represent in opengl
    float data[] = 
    {
         // vertices
         0.5f, -0.5f,  0.0f, //v1
         0.5f,  0.5f,  0.0f, //v2
         0.0f,  0.0f,  1.0f, //v5

         0.5f,  0.5f,  0.0f, //v2
        -0.5f,  0.5f,  0.0f, //v3
         0.0f,  0.0f,  1.0f, //v5

        -0.5f,  0.5f,  0.0f, //v3
        -0.5f, -0.5f,  0.0f, //v4
         0.0f,  0.0f,  1.0f, //v5

        -0.5f, -0.5f,  0.0f, //v4
         0.5f, -0.5f,  0.0f, //v1
         0.0f,  0.0f,  1.0f, //v5

         0.5f,  0.5f,  0.0f, //v2
         0.5f, -0.5f,  0.0f, //v1
         0.0f,  0.0f, -1.0f, //v6

        -0.5f,  0.5f,  0.0f, //v3
         0.5f,  0.5f,  0.0f, //v2
         0.0f,  0.0f, -1.0f, //v6

        -0.5f, -0.5f,  0.0f, //v4
        -0.5f,  0.5f,  0.0f, //v3
         0.0f,  0.0f, -1.0f, //v6

         0.5f, -0.5f,  0.0f, //v1
        -0.5f, -0.5f,  0.0f, //v4
         0.0f,  0.0f, -1.0f, //v6

         // normals
         0.894427191f,  0.0f,  0.447213595f, //n1
         0.894427191f,  0.0f,  0.447213595f, //n1
         0.894427191f,  0.0f,  0.447213595f, //n1

         0.0f,  0.894427191f,  0.447213595f, //n2
         0.0f,  0.894427191f,  0.447213595f, //n2
         0.0f,  0.894427191f,  0.447213595f, //n2

        -0.894427191f,  0.0f,  0.447213595f, //n3
        -0.894427191f,  0.0f,  0.447213595f, //n3
        -0.894427191f,  0.0f,  0.447213595f, //n3

         0.0f, -0.894427191f,  0.447213595f, //n4
         0.0f, -0.894427191f,  0.447213595f, //n4
         0.0f, -0.894427191f,  0.447213595f, //n4

         0.894427191f,  0.0f, -0.447213595f, //n5
         0.894427191f,  0.0f, -0.447213595f, //n5
         0.894427191f,  0.0f, -0.447213595f, //n5

         0.0f,  0.894427191f, -0.447213595f, //n6
         0.0f,  0.894427191f, -0.447213595f, //n6
         0.0f,  0.894427191f, -0.447213595f, //n6

        -0.894427191f,  0.0f, -0.447213595f, //n7
        -0.894427191f,  0.0f, -0.447213595f, //n7
        -0.894427191f,  0.0f, -0.447213595f, //n7

         0.0f, -0.894427191f, -0.447213595f, //n8
         0.0f, -0.894427191f, -0.447213595f, //n8
         0.0f, -0.894427191f, -0.447213595f, //n8
    };

    // scale the vertex locations relative to the scale
    // parameter (note I only go to 72, the num verts)
    for (int i = 0; i < 72; i++)
    {
        data[i] *= scale;
    }

    // since we have redefined our vertices, the indices
    // are just a linear ordering
    int indices[24] = {0,1,2,3,4,5,6,7,8,9,10,11,12,
        13,14,15,16,17,18,19,20,21,22,23};

    // attribute information for vertices
    details[0].attrib_number = 0;
    details[0].attrib_size   = 3 * sizeof(float); // size of one vertex
    details[0].data_offset   = 0; // byte offset to vertices
    details[0].data_stride   = 0; // space between vertices
    details[0].num_comp      = 3;

    // attribute information for normals
    details[1].attrib_number = 1;
    details[1].attrib_size   = 3 * sizeof(float); // size of one normal
    details[1].data_offset   = 24 * 3 * sizeof(float); // byte offset to normals
    details[1].data_stride   = 0; // space between normals
    details[1].num_comp      = 3;

    m = new Mesh();
    m->init(1, state);
    m->loadVBuffer(0, sizeof(data), (GLubyte*)data, 0, 2, details);
    m->loadIBuffer(24, sizeof(int), indices);

    return m;
}

// returns a color indicating which quadtrant the modelview tranform has
// translated the point (0,0,0) to.
glm::vec3 identifyQuadrant(glm::mat4 modelview)
{
    glm::vec3 out = glm::vec3();
    out.r = (modelview[3].r > 0) ? 1 : 0;
    out.g = (modelview[3].g > 0) ? 1 : 0;
    out.b = (modelview[3].b > 0) ? 1 : 0;
    out *= 0.7;
    out += glm::vec3(0.3);
    return out;
}
#endif // MESH_UTILS_H