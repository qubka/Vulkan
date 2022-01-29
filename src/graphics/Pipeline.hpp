#pragma once

namespace Engine {
    class Device;

    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo(PipelineConfigInfo&&) = default;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(PipelineConfigInfo&&) = delete;

        /*std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
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
        uint32_t subpass{0};*/
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
        vk::Viewport viewport = {};
        vk::Rect2D scissor = {};
        vk::PipelineViewportStateCreateInfo viewportState = {};
        vk::PipelineRasterizationStateCreateInfo rasterizer = {};
        vk::PipelineMultisampleStateCreateInfo multisampling = {};
        vk::PipelineColorBlendStateCreateInfo colorBlending = {};
        vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
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

        //static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
    private:
        Device& device;
        vk::Pipeline graphicsPipeline;

        vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);
        void createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);

        static std::vector<char> readFile(const std::string& path);
    };
}
