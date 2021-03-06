/*MIT License

Copyright (c) 2021 Mohammad Issawi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#pragma once
#include <simple-animation-blender/Application.h>
#include <simple-animation-blender/Mesh.h>
class Animator
{
private:
    Animator();
    Mesh *mesh;
    Mesh::Animation playingAnimationOne, playingAnimationTwo;
    double animationTime;
    float blendingFactor = -1;
    //Updates mesh nodes and bones transformations for a single animation at some time
    void updateMeshNodes(const aiNode *node, const glm::mat4 &parentTransform);
    //Updates mesh nodes and bones transformations by blending two animations at some time
    //if both animations are not lengthly equal, the longer one will be considered the blended
    //animation time and the other will stay in the last key until the animation loops
    void blendMeshNodes(const aiNode *node, const glm::mat4 &parentTransform);
    //Gets key index at some time for the provided channel
    int getKeyIndex(std::vector<Mesh::PosKey> keys, Mesh::Channel channel);
    int getKeyIndex(std::vector<Mesh::RotKey> keys, Mesh::Channel channel);
    int getKeyIndex(std::vector<Mesh::ScaKey> keys, Mesh::Channel channel);
    //Lineary interpolates two animation position keys at some time for the provided channel
    glm::vec3 interpolatePos(Mesh::Channel channel);
    //Uses Assimp interpolating algorithm for interpolating two animation quaternion keys at
    //some time for the provided channel
    aiQuaternion interpolateRot(Mesh::Channel channel);
    //Lineary interpolates two animation scale keys at some time for the provided channel
    glm::vec3 interpolateSca(Mesh::Channel channel);

public:
    static Animator &instance();
    bool isPlaying = false;
    void init(Mesh *mesh);
    void play(std::string animation);
    void play(std::string firstAnim, std::string secondAnim, float blendingFactor);
    void animate(double timeSinceStart);
};