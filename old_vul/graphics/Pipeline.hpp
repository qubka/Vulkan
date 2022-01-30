#pragma once

namespace Engine {
    class Device;

    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo(PipelineConfigInfo&&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(PipelineConfigInfo&&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout{nullptr};
        VkRenderPass renderPass{nullptr};
        uint32_t subpass{0};
    };

    class Pipeline {
    public:
        Pipeline(Device& device,
                 const std::string& vertPath,
                 const std::string& fragPath,
                 const PipelineConfigInfo& configInfo);

        ~Pipeline();
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&&) = delete;

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

        void bind(VkCommandBuffer commandBuffer) const;

        operator VkPipeline() const;

    private:
        Device& device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;

        void createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
        static std::vector<char> readFile(const std::string& path);
    };
}
