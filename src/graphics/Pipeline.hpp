#pragma once

namespace Engine {
    class Device;

    struct PipelineConfigInfo {
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        vk::PipelineViewportStateCreateInfo viewportInfo{};
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
        vk::PipelineMultisampleStateCreateInfo multisampleInfo{};
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
        std::vector<vk::DynamicState> dynamicStateEnables{};
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        vk::PipelineLayout pipelineLayout{nullptr};
        vk::RenderPass renderPass{nullptr};
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

        void bind(const vk::CommandBuffer& commandBuffer) const;

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
    private:
        Device& device;
        vk::UniquePipeline graphicsPipeline;

        vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
        void createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);

        static std::vector<char> readFile(const std::string& path);
    };
}
