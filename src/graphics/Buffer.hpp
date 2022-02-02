#pragma once

namespace Engine {
    class Device;

    class Buffer {
    public:
        Buffer(Device& device,
               vk::DeviceSize instanceSize,
               uint32_t instanceCount,
               vk::BufferUsageFlags usageFlags,
               vk::MemoryPropertyFlags memoryPropertyFlags,
               vk::DeviceSize minOffsetAlignment = 1);
        ~Buffer();
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&&) = delete;

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

        vk::Buffer& getBuffer();
        void* getMappedMemory();

        uint32_t getInstanceCount() const;
        vk::DeviceSize getInstanceSize() const;
        vk::DeviceSize getAlignmentSize() const;
        vk::BufferUsageFlags getUsageFlags() const;
        vk::MemoryPropertyFlags getMemoryPropertyFlags() const;
        vk::DeviceSize getBufferSize() const;

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

