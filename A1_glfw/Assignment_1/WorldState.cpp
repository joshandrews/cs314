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

#include "WorldState.h"

WorldState::WorldState()
{
    // make sure that the world can hold at least 5 of each object
    // before it needs to reallocate memory
    shaders.reserve(5);
    lights.reserve(5);
    materials.reserve(5);

    // start with one light and material by default
    lights.push_back(LightInfo());
    materials.push_back(MaterialInfo());
}

WorldState::~WorldState()
{}

void WorldState::useProgram(unsigned int i)
{
    glUseProgram(shaders[i]);
    currentProgram = i;
}

void WorldState::setProgram(unsigned int i, GLuint program)
{
    while (shaders.size() <= i)
        shaders.push_back((GLuint)0);

    shaders[i] = program;
}

void WorldState::loadTransforms()
{
    loadTransforms(modelview, projection);
}

void WorldState::loadTransforms(glm::mat4 MV)
{
    loadTransforms(MV, projection);
}

void WorldState::loadTransforms(glm::mat4 MV, glm::mat4 P)
{
    // setup uniform variables
    glUniformMatrix4fv(glGetUniformLocation(shaders[currentProgram],"ModelViewMatrix"),1, GL_FALSE, glm::value_ptr(MV));
    glUniformMatrix4fv(glGetUniformLocation(shaders[currentProgram],"ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(glGetUniformLocation(shaders[currentProgram],"MVP"), 1, GL_FALSE, glm::value_ptr(P * MV));

    /* the Normal Matrix is the inverse transpose of the upper left 3x3 modelview matrix
     * this is used to transform normals, as they do not transform the same way as
     * vertices. */
    glUniformMatrix3fv(glGetUniformLocation(shaders[currentProgram],"NormalMatrix"), 1, GL_TRUE, glm::value_ptr(glm::inverse(glm::mat3(MV))));
}

void WorldState::loadObjectTransforms(glm::mat4 oMV)
{
    loadTransforms(modelview * oMV, projection);
}

void WorldState::loadLight(unsigned int i)
{
    glm::normalize(lights[i].Position);
    glUniform4fv(glGetUniformLocation(shaders[currentProgram], "Light.Position"), 1, glm::value_ptr(lights[i].Position));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Light.La"), 1, glm::value_ptr(lights[i].La));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Light.Ld"), 1, glm::value_ptr(lights[i].Ld));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Light.Ls"), 1, glm::value_ptr(lights[i].Ls));
}

void WorldState::loadLights()
{
    // TODO: each load light currently overwrites itself, need to fix shaders and loadLight(i)
    for (unsigned int i = 0; i < lights.size(); i++)
        loadLight(i);
}

void WorldState::loadMaterial(unsigned int i)
{
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Material.Ka"), 1, glm::value_ptr(materials[i].Ka));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Material.Kd"), 1, glm::value_ptr(materials[i].Kd));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Material.Ks"), 1, glm::value_ptr(materials[i].Ks));
    glUniform1f(glGetUniformLocation(shaders[currentProgram], "Material.Shininess"), materials[i].Shininess);
}

void WorldState::loadMaterials()
{
    // TODO: each load material currently overwrites itself, need to fix shaders and loadMaterial(i)
    for (unsigned int i = 0; i < materials.size(); i++)
        loadMaterial(i);
}

void WorldState::loadColorMaterial(glm::vec4 color)
{
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Material.Ka"), 1, glm::value_ptr(glm::vec3(color)));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Material.Kd"), 1, glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
    glUniform3fv(glGetUniformLocation(shaders[currentProgram], "Material.Ks"), 1, glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
    glUniform1f(glGetUniformLocation(shaders[currentProgram], "Material.Shininess"), 0.5f);
}