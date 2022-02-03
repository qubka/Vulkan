#pragma once

namespace Engine {
    class Device;

    class AllocatedBuffer {
    public:
        AllocatedBuffer(Device& device,
                        vk::DeviceSize instanceSize,
                        uint32_t instanceCount,
                        vk::BufferUsageFlags usageFlags,
                        vk::MemoryPropertyFlags memoryPropertyFlags,
                        vk::DeviceSize minOffsetAlignment = 1);
        ~AllocatedBuffer();
        AllocatedBuffer(const AllocatedBuffer&) = delete;
        AllocatedBuffer(AllocatedBuffer&&) = delete;
        AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;
        AllocatedBuffer& operator=(AllocatedBuffer&&) = delete;

        void map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        void unmap();

        void writeToBuffer(void* data, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        vk::Result flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        vk::DescriptorBufferInfo descriptorInfo(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
        vk::Result invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

        void writeToIndex(void* data, int index);
        vk::Result flushIndex(int index);
        vk::DescriptorBufferInfo descriptorInfoForIndex(int index);
        vk::Result invalidateIndex(int index);

        vk::Buffer& operator*() { return buffer; };
        vk::Buffer& get() { return buffer; };
        void* getMappedMemory() { return mapped; };
        uint32_t getInstanceCount() const { return instanceSize; };
        vk::DeviceSize getInstanceSize() const { return instanceSize; };
        vk::DeviceSize getAlignmentSize() const { return alignmentSize; };
        vk::BufferUsageFlags getUsageFlags() const { return usageFlags; };
        vk::MemoryPropertyFlags getMemoryPropertyFlags()  { return memoryPropertyFlags; };
        vk::DeviceSize getBufferSize() const { return bufferSize; };

    private:
        Device& device;
        void* mapped = nullptr;
        vk::Buffer buffer;
        vk::DeviceMemory memory;

        vk::DeviceSize bufferSize;
        uint32_t instanceCount;
        vk::DeviceSize instanceSize;
        vk::DeviceSize alignmentSize;
        vk::BufferUsageFlags usageFlags;
        vk::MemoryPropertyFlags memoryPropertyFlags;

        static vk::DeviceSize getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);
    };
}

