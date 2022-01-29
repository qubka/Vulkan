#include "Pipeline.hpp"
#include "Device.hpp"

using Engine::Pipeline;
using Engine::PipelineConfigInfo;

Pipeline::Pipeline(Engine::Device& device, const std::string& vertPath, const std::string& fragPath, const Engine::PipelineConfigInfo& configInfo) : device{device} {
    createGraphicsPipeline(vertPath, fragPath, configInfo);
}

Pipeline::~Pipeline() {
    device()->destroyPipeline(graphicsPipeline);
}

void Pipeline::createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo) {
    auto vertShaderCode = readFile(vertPath);
    auto fragShaderCode = readFile(fragPath);

    auto vertShaderModule = createShaderModule(vertShaderCode);
    auto fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eVertex,
            *vertShaderModule,
            "main"
        },
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eFragment,
            *fragShaderModule,
            "main"
        }
    };

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &configInfo.vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssembly;
    pipelineInfo.pViewportState = &configInfo.viewportState;
    pipelineInfo.pRasterizationState = &configInfo.rasterizer;
    pipelineInfo.pMultisampleState = &configInfo.multisampling;
    pipelineInfo.pColorBlendState = &configInfo.colorBlending;
    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;
    pipelineInfo.basePipelineHandle = nullptr;

    try {
        graphicsPipeline = device()->createGraphicsPipeline(nullptr, pipelineInfo).value;
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

vk::UniqueShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
    try {
        return device()->createShaderModuleUnique({
            vk::ShaderModuleCreateFlags(),
            code.size(),
            reinterpret_cast<const uint32_t*>(code.data())
        });
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create shader module!");
    }
}

std::vector<char> Pipeline::readFile(const std::string& path) {
    std::ifstream file {path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    size_t size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(size);

    file.seekg(0);
    file.read(buffer.data(), size);

    file.close();

    return buffer;
}

void Pipeline::bind(const vk::CommandBuffer& commandBuffer) const {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
}

PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo configInfo{};

    configInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    configInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;

    configInfo.inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    configInfo.viewport.x = 0.0f;
    configInfo.viewport.y = 0.0f;
    configInfo.viewport.width = (float)width;
    configInfo.viewport.height = (float)height;
    configInfo.viewport.minDepth = 0.0f;
    configInfo.viewport.maxDepth = 1.0f;

    configInfo.scissor.offset = VkOffset2D{ 0, 0 };
    configInfo.scissor.extent = VkExtent2D{width, height};

    configInfo.viewportState.viewportCount = 1;
    configInfo.viewportState.pViewports = &configInfo.viewport;
    configInfo.viewportState.scissorCount = 1;
    configInfo.viewportState.pScissors = &configInfo.scissor;

    configInfo.rasterizer.depthClampEnable = VK_FALSE;
    configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizer.polygonMode = vk::PolygonMode::eFill;
    configInfo.rasterizer.lineWidth = 1.0f;
    configInfo.rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    configInfo.rasterizer.frontFace = vk::FrontFace::eClockwise;
    configInfo.rasterizer.depthBiasEnable = VK_FALSE;

    configInfo.multisampling.sampleShadingEnable = VK_FALSE;
    configInfo.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    configInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;

    configInfo.colorBlending.logicOpEnable = VK_FALSE;
    configInfo.colorBlending.logicOp = vk::LogicOp::eCopy;
    configInfo.colorBlending.attachmentCount = 1;
    configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlending.blendConstants[0] = 0.0f;
    configInfo.colorBlending.blendConstants[1] = 0.0f;
    configInfo.colorBlending.blendConstants[2] = 0.0f;
    configInfo.colorBlending.blendConstants[3] = 0.0f;

    return configInfo;
}


/*void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {
    configInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.pViewports = nullptr;
    configInfo.viewportInfo.scissorCount = 1;
    configInfo.viewportInfo.pScissors = nullptr;

    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
    configInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
    configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    configInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;   // Optional
    configInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;  // Optional
    configInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;              // Optional
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;   // Optional
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;  // Optional
    configInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;              // Optional

    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy;
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    //configInfo.depthStencilInfo.front = vk::StencilOp::eZero;  // Optional
    //configInfo.depthStencilInfo.back = vk::StencilOp::eZero;   // Optional

    configInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
    //configInfo.dynamicStateInfo.flags = 0;

    configInfo.bindingDescriptions = Mesh::Vertex::getBindingDescriptions();
    configInfo.attributeDescriptions = Mesh::Vertex::getAttributeDescriptions();
}*/
