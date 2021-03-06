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
#include <simple-animation-blender/Application.h>
#include <simple-animation-blender/GUI.h>
#include <simple-animation-blender/Animator.h>
#include <GLFW/glfw3.h>
using namespace std;
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        ERR("Mesh data was not provided, please provide it as a CLI argument");
    }
    Application::instance().init(argv[1]);
    GUI::instance().init();
    Animator::instance().init(Application::instance().mesh);
    //GUI values holders
    float colorHolder[3] = {1.0f, 0.0f, 0.0f};
    int firstAnimationHolder = 0, secondAnimationHolder = 0;
    float blendingFactorHolder = 0.5f;
    string animationsList("None");
    animationsList.append(1, '\0');
    for (uint32_t i = 0; i < Application::instance().mesh->animations.size(); ++i)
    {
        animationsList.append(Application::instance().mesh->animations[i].name);
        animationsList.append(1, '\0');
    }
    {
        VkSemaphore acquireSemaphore{};
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(Application::instance().device, &semaphoreCreateInfo, ALLOCATOR, &acquireSemaphore) != VK_SUCCESS)
        {
            ERR("Failed to create semaphore");
        }
        VkFence fence{};
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(Application::instance().device, &fenceCreateInfo, ALLOCATOR, &fence) != VK_SUCCESS)
        {
            ERR("Failed to create fence");
        }
        VkSemaphore presentSemaphore;
        if (vkCreateSemaphore(Application::instance().device, &semaphoreCreateInfo, ALLOCATOR, &presentSemaphore) != VK_SUCCESS)
        {
            ERR("Failed to create semaphore");
        }
        while (!glfwWindowShouldClose(Application::instance().window))
        {
            glfwPollEvents();
            GUI::instance().updateColor(colorHolder);
            GUI::instance().updateAnimatorData(firstAnimationHolder, secondAnimationHolder, blendingFactorHolder);

            //ImGui representation info
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Simple Animation Blender");
            ImGui::Combo("First Animation", &firstAnimationHolder, animationsList.c_str());
            if (firstAnimationHolder)
                ImGui::Combo("Second Animation", &secondAnimationHolder, animationsList.c_str());
            if (firstAnimationHolder && secondAnimationHolder)
                ImGui::SliderFloat("Blending Factor", &blendingFactorHolder, 0.0f, 1.0f, "%.3f", 1.0f);
            ImGui::ColorEdit3("Mesh Color", colorHolder);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("+/- for zooming");
            ImGui::Separator();
            ImGui::Text("Achieved with Slowly Walking Kittens");
            ImGui::Text("Simple Animation Blender by Mohammad Issawi a.k.a RedDeadAlice");
            ImGui::End();
            ImGui::Render();
            ImDrawData *imGuiDrawData = ImGui::GetDrawData();

            Animator::instance().animate(glfwGetTime());

            vkWaitForFences(Application::instance().device, 1, &fence, VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;
            vkAcquireNextImageKHR(Application::instance().device, Application::instance().swapchain, 0, acquireSemaphore, VK_NULL_HANDLE, &imageIndex);
            {
                if (vkResetCommandPool(Application::instance().device, Application::instance().cmdPool, 0) != VK_SUCCESS)
                {
                    ERR("Failed to reset cmd pool");
                }
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                VkRenderPassBeginInfo rpBeginInfo{};
                rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                rpBeginInfo.renderPass = Application::instance().renderPass;
                rpBeginInfo.framebuffer = Application::instance().framebuffers[imageIndex].vkHandle;
                rpBeginInfo.clearValueCount = 2;
                VkClearValue clearValues[2];
                clearValues[0].color = {0.25f, 0.25f, 0.25f, 1};
                clearValues[1].depthStencil.depth = 1.0f;
                rpBeginInfo.pClearValues = clearValues;
                rpBeginInfo.renderArea.extent.height = Application::instance().fbHeight;
                rpBeginInfo.renderArea.extent.width = Application::instance().fbWidth;
                rpBeginInfo.renderArea.offset = {0, 0};
                VkDeviceSize offsets = 0;
                if (vkBeginCommandBuffer(Application::instance().cmdBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
                {
                    ERR(FORMAT("Failed to create cmd buffer {}", imageIndex));
                }
                vkCmdBeginRenderPass(Application::instance().cmdBuffers[imageIndex], &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(Application::instance().cmdBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Application::instance().pipeline);
                vkCmdBindVertexBuffers(Application::instance().cmdBuffers[imageIndex], 0, 1, &(Application::instance().mesh->vertexBuffer), &offsets);
                vkCmdBindIndexBuffer(Application::instance().cmdBuffers[imageIndex], Application::instance().mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
                vkCmdBindDescriptorSets(Application::instance().cmdBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Application::instance().pipelineLayout, 0, 1, &(Application::instance().mesh->descriptorSet), 0, nullptr);
                vkCmdDrawIndexed(Application::instance().cmdBuffers[imageIndex], Application::instance().mesh->indices.size(), 1, 0, 0, 0);
                ImGui_ImplVulkan_RenderDrawData(imGuiDrawData, Application::instance().cmdBuffers[imageIndex]);
                vkCmdEndRenderPass(Application::instance().cmdBuffers[imageIndex]);
                if (vkEndCommandBuffer(Application::instance().cmdBuffers[imageIndex]) != VK_SUCCESS)
                {
                    ERR(FORMAT("Failed to record cmd buffer {}", imageIndex));
                }
            }
            VkSubmitInfo submitInfo{};
            submitInfo.commandBufferCount = 1;
            VkCommandBuffer cmdBuffer = Application::instance().cmdBuffers[imageIndex];
            submitInfo.pCommandBuffers = &cmdBuffer;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &acquireSemaphore;
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
            submitInfo.pWaitDstStageMask = waitStage;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &presentSemaphore;
            vkResetFences(Application::instance().device, 1, &fence);
            if (vkQueueSubmit(Application::instance().graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
            {
                ERR("Failed to submit graphics render");
            }
            VkPresentInfoKHR presentInfo{};
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &presentSemaphore;
            presentInfo.swapchainCount = 1;
            VkSwapchainKHR swapchain = Application::instance().swapchain;
            presentInfo.pSwapchains = &swapchain;
            if (vkQueuePresentKHR(Application::instance().graphicsQueue, &presentInfo) != VK_SUCCESS)
            {
                ERR("Failed to present to swapchain");
            }
        }
        vkQueueWaitIdle(Application::instance().graphicsQueue);
        vkDestroyFence(Application::instance().device, fence, ALLOCATOR);
        vkDestroySemaphore(Application::instance().device, acquireSemaphore, ALLOCATOR);
        vkDestroySemaphore(Application::instance().device, presentSemaphore, ALLOCATOR);
    }
    GUI::instance().terminate();
    Application::instance().terminate();
}