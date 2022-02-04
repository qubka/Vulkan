#pragma once

namespace Engine {
    class Device;

    class DescriptorPool {
    public:
        class Builder {
        public:
            explicit Builder(Device& device) : device{device} {}

            Builder& addPoolSize(vk::DescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(vk::DescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<DescriptorPool> build() const;

        private:
            Device& device;
            std::vector<vk::DescriptorPoolSize> poolSizes;
            vk::DescriptorPoolCreateFlags poolFlags;
            uint32_t maxSets{1000};

            friend class DescriptorPool;
        };

        DescriptorPool(const Builder& builder);
        ~DescriptorPool();
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool(DescriptorPool&&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;
        DescriptorPool& operator=(DescriptorPool&&) = delete;

        bool allocateDescriptor(const vk::DescriptorSetLayout& setLayout, vk::DescriptorSet& descriptor) const;
        void freeDescriptors(std::vector<vk::DescriptorSet>& descriptors) const;
        void resetPool();

    private:
        Device& device;
        vk::DescriptorPool descriptorPool;

        friend class DescriptorWriter;
    };

    class DescriptorLayout {
    public:
        class Builder {
        public:
            explicit Builder(Device& device) : device{device} {}
            Builder& addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count = 1);
            std::unique_ptr<DescriptorLayout> build() const;

        private:
            Device& device;
            std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;

            friend class DescriptorLayout;
        };

        DescriptorLayout(const Builder& builder);
        ~DescriptorLayout();
        DescriptorLayout(const DescriptorLayout&) = delete;
        DescriptorLayout(DescriptorLayout&&) = delete;
        DescriptorLayout& operator=(const DescriptorLayout&) = delete;
        DescriptorLayout& operator=(DescriptorLayout&&) = delete;

        const vk::DescriptorSetLayout& getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Device& device;
        vk::DescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;

        friend class DescriptorWriter;
    };

    class DescriptorWriter {
    public:
        DescriptorWriter(DescriptorLayout& layout, DescriptorPool& pool);
        DescriptorWriter& writeBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo);
        DescriptorWriter& writeImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo);

        bool build(vk::DescriptorSet& set);
        void overwrite(vk::DescriptorSet& set);

    private:
        DescriptorLayout& layout;
        DescriptorPool& pool;
        std::vector<vk::WriteDescriptorSet> writes;
    };

    /*class DescriptorAllocator {
    public:
        struct PoolSizes {
            std::vector<std::pair<vk::DescriptorType, float>> sizes =
                {
                    { vk::DescriptorType::eSampler, 0.5f },
                    { vk::DescriptorType::eCombinedImageSampler, 4.f },
                    { vk::DescriptorType::eSampledImage, 4.f },
                    { vk::DescriptorType::eStorageImage, 1.f },
                    { vk::DescriptorType::eUniformTexelBuffer, 1.f },
                    { vk::DescriptorType::eStorageTexelBuffer, 1.f },
                    { vk::DescriptorType::eUniformBuffer, 2.f },
                    { vk::DescriptorType::eStorageBuffer, 2.f },
                    { vk::DescriptorType::eUniformBufferDynamic, 1.f },
                    { vk::DescriptorType::eStorageBufferDynamic, 1.f },
                    { vk::DescriptorType::eInputAttachment, 0.5f }
                };
        };

        DescriptorAllocator(Device& device);
        ~DescriptorAllocator();
        DescriptorAllocator(const DescriptorAllocator&) = delete;
        DescriptorAllocator(DescriptorAllocator&&) = delete;
        DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;
        DescriptorAllocator& operator=(DescriptorAllocator&&) = delete;

        vk::DescriptorPool grabPool();
        void resetPools();
        bool allocateDescriptor(const vk::DescriptorSetLayout& layout, vk::DescriptorSet& set);
        void updateDescriptor(std::vector<vk::WriteDescriptorSet>& writes) const;

    private:
        vk::DescriptorPool createPool(uint32_t count, vk::DescriptorPoolCreateFlags flags) const;

        vk::DescriptorPool currentPool{nullptr};
        PoolSizes descriptorSizes;
        std::vector<vk::DescriptorPool> usedPools;
        std::vector<vk::DescriptorPool> freePools;

        Device& device;
    };

    class DescriptorLayoutCache {
    public:
        DescriptorLayoutCache(Device& device);
        ~DescriptorLayoutCache();
        DescriptorLayoutCache(const DescriptorLayoutCache&) = delete;
        DescriptorLayoutCache(DescriptorLayoutCache&&) = delete;
        DescriptorLayoutCache& operator=(const DescriptorLayoutCache&) = delete;
        DescriptorLayoutCache& operator=(DescriptorLayoutCache&&) = delete;

        vk::DescriptorSetLayout createDescriptorLayout(vk::DescriptorSetLayoutCreateInfo& info);

        struct DescriptorLayoutInfo {
            //good idea to turn this into an inlined array
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            bool operator==(const DescriptorLayoutInfo& other) const;
            size_t hash() const;
        };

    private:
        struct DescriptorLayoutHash {
            std::size_t operator()(const DescriptorLayoutInfo& k) const { return k.hash(); }
        };

        std::unordered_map<DescriptorLayoutInfo, vk::DescriptorSetLayout, DescriptorLayoutHash> layoutCache;
        Device& device;
    };

    class DescriptorBuilder {
    public:
        DescriptorBuilder(DescriptorLayoutCache& cache, DescriptorAllocator& allocator);
        ~DescriptorBuilder();
        DescriptorBuilder(const DescriptorBuilder&) = delete;
        DescriptorBuilder(DescriptorBuilder&&) = delete;
        DescriptorBuilder& operator=(const DescriptorBuilder&) = delete;
        DescriptorBuilder& operator=(DescriptorBuilder&&) = delete;

        DescriptorBuilder& bindBuffer(uint32_t binding, vk::DescriptorBufferInfo& bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);
        DescriptorBuilder& bindImage(uint32_t binding, vk::DescriptorImageInfo& imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        bool build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
        bool build(vk::DescriptorSet& set);
    private:

        std::vector<vk::WriteDescriptorSet> writes;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        DescriptorLayoutCache& cache;
        DescriptorAllocator& allocator;
    };*/
};
