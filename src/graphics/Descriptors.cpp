#include "Descriptors.hpp"
#include "Device.hpp"

using Engine::DescriptorLayout;
using Engine::DescriptorPool;
using Engine::DescriptorWriter;

// *************** Descriptor Pool Builder *********************

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(vk::DescriptorType descriptorType, uint32_t count) {
    poolSizes.emplace_back(descriptorType, count);
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(vk::DescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count) {
    maxSets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(*this);
}

// *************** Descriptor Pool *********************

DescriptorPool::DescriptorPool(const Builder& builder) : device{builder.device} {
    vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(builder.poolSizes.size());
    descriptorPoolInfo.pPoolSizes = builder.poolSizes.data();
    descriptorPoolInfo.maxSets = builder.maxSets;
    descriptorPoolInfo.flags = builder.poolFlags;

    try {
        descriptorPool = device.getLogical().createDescriptorPool(descriptorPoolInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

DescriptorPool::~DescriptorPool() {
    device.getLogical().destroyDescriptorPool(descriptorPool);
}

bool DescriptorPool::allocateDescriptor(const vk::DescriptorSetLayout& setLayout, vk::DescriptorSet& descriptor) const {
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &setLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (device.getLogical().allocateDescriptorSets(&allocInfo, &descriptor) != vk::Result::eSuccess) {
        std::cerr << "pools fills up!" << std::endl;
        return false;
    }

    return true;
}

void DescriptorPool::freeDescriptors(std::vector<vk::DescriptorSet>& descriptorSets) const {
    device.getLogical().freeDescriptorSets(descriptorPool, descriptorSets);
}

void DescriptorPool::resetPool() {
    device.getLogical().resetDescriptorPool(descriptorPool);
}

// *************** Descriptor Set Layout Builder *********************

DescriptorLayout::Builder& DescriptorLayout::Builder::addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count) {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    vk::DescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorLayout> DescriptorLayout::Builder::build() const {
    return std::make_unique<DescriptorLayout>(*this);
}

// *************** Descriptor Set Layout *********************

DescriptorLayout::DescriptorLayout(const Builder& builder) : device{builder.device}, bindings{builder.bindings} {
    std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings;
    setLayoutBindings.reserve(bindings.size());

    for (const auto& b : bindings) {
        setLayoutBindings.push_back(b.second);
    }

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    try {
        descriptorSetLayout = device.getLogical().createDescriptorSetLayout(descriptorSetLayoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

DescriptorLayout::~DescriptorLayout() {
    device.getLogical().destroyDescriptorSetLayout(descriptorSetLayout);
}

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(DescriptorLayout& layout, DescriptorPool& pool) : layout{layout}, pool{pool} {
}

DescriptorWriter& DescriptorWriter::writeBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo) {
    assert(layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
    const auto& bindingDescription = layout.bindings[binding];
    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    vk::WriteDescriptorSet write{};
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo) {
    assert(layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
    const auto& bindingDescription = layout.bindings[binding];
    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    vk::WriteDescriptorSet write{};
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = &imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(vk::DescriptorSet& set) {
    bool success = pool.allocateDescriptor(layout.getDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(vk::DescriptorSet& set) {
    for (auto& write: writes) {
        write.dstSet = set;
    }

    pool.device.getLogical().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

/*using Engine::DescriptorAllocator;
using Engine::DescriptorLayoutCache;
using Engine::DescriptorBuilder;

DescriptorAllocator::DescriptorAllocator(Device& device) : device{device} {
}

DescriptorAllocator::~DescriptorAllocator() {
    //delete every pool held
    for (const auto& p : freePools) {
        device.getLogical().destroyDescriptorPool(p);
    }
    for (const auto& p : usedPools) {
        device.getLogical().destroyDescriptorPool(p);
    }
}

vk::DescriptorPool DescriptorAllocator::grabPool() {
    if (!freePools.empty()) {
        //grab pool from the back of the vector and remove it from there.
        vk::DescriptorPool pool = freePools.back();
        freePools.pop_back();
        return pool;
    } else {
        //no pools availible, so create a new one
        return createPool(1000, vk::DescriptorPoolCreateFlags());
    }
}

vk::DescriptorPool DescriptorAllocator::createPool(uint32_t count, vk::DescriptorPoolCreateFlags flags) const {
    std::vector<vk::DescriptorPoolSize> sizes;
    sizes.reserve(descriptorSizes.sizes.size());

    for (auto sz : descriptorSizes.sizes) {
        sizes.emplace_back(sz.first, static_cast<uint32_t>(sz.second * count));
    }

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.flags = flags;
    poolInfo.maxSets = count;
    poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    poolInfo.pPoolSizes = sizes.data();

    try {
        return device.getLogical().createDescriptorPool(poolInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

bool DescriptorAllocator::allocateDescriptor(const vk::DescriptorSetLayout& layout, vk::DescriptorSet& set) {
    //initialize the currentPool handle if it's null
    if (!currentPool) {
        currentPool = grabPool();
        usedPools.push_back(currentPool);
    }

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.pNext = nullptr;
    allocInfo.pSetLayouts = &layout;
    allocInfo.descriptorPool = currentPool;
    allocInfo.descriptorSetCount = 1;

    //try to allocate the descriptor set
    auto result = device.getLogical().allocateDescriptorSets(&allocInfo, &set);

    switch (result) {
        case vk::Result::eSuccess:
            //all good, return
            return true;
        case vk::Result::eErrorFragmentedPool:
        case vk::Result::eErrorOutOfPoolMemory:
            //allocate a new pool and retry
            currentPool = grabPool();
            usedPools.push_back(currentPool);

            result = device.getLogical().allocateDescriptorSets(&allocInfo, &set);

            //if it still fails then we have big issues
            if (result == vk::Result::eSuccess){
                return true;
            }
            break;
        default:
            //unrecoverable error
            return false;
    }

    return false;
}

void DescriptorAllocator::resetPools() {
    //reset all used pools and add them to the free pools
    for (const auto& p : usedPools) {
        device.getLogical().resetDescriptorPool(p);
        freePools.push_back(p);
    }

    //clear the used pools, since we've put them all in the free pools
    usedPools.clear();

    //reset the current pool handle back to null
    currentPool = nullptr;
}

void DescriptorAllocator::updateDescriptor(std::vector<vk::WriteDescriptorSet>& writes) const {
    device.getLogical().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

DescriptorLayoutCache::DescriptorLayoutCache(Device& device) : device{device} {
}

DescriptorLayoutCache::~DescriptorLayoutCache() {
    //delete every descriptor layout held
    for (const auto& [info, layout] : layoutCache) {
        device.getLogical().destroyDescriptorSetLayout(layout);
    }
}

vk::DescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(vk::DescriptorSetLayoutCreateInfo& info) {
    DescriptorLayoutInfo layoutinfo;
    layoutinfo.bindings.reserve(info.bindingCount);
    bool isSorted = true;
    int32_t lastBinding = -1;

    //copy from the direct info struct into our own one
    for (int i = 0; i < info.bindingCount; i++) {
        layoutinfo.bindings.push_back(info.pBindings[i]);

        //check that the bindings are in strict increasing order
        int binding = static_cast<int32_t>(info.pBindings[i].binding);
        if (binding > lastBinding) {
            lastBinding = binding;
        } else {
            isSorted = false;
        }
    }
    //sort the bindings if they aren't in order
    if (!isSorted) {
        std::sort(layoutinfo.bindings.begin(), layoutinfo.bindings.end(),
              [](const vk::DescriptorSetLayoutBinding& a, const vk::DescriptorSetLayoutBinding& b) {
                  return a.binding < b.binding;
              });
    }

    //try to grab from cache
    auto it = layoutCache.find(layoutinfo);
    if (it != layoutCache.end()) {
        return (*it).second;
    } else {
        //create a new one (not found)
        vk::DescriptorSetLayout layout;

        try {
            layout = device.getLogical().createDescriptorSetLayout(info);
        } catch (vk::SystemError& err) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        //add to cache
        layoutCache[layoutinfo] = layout;
        return layout;
    }
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const{
    if (other.bindings.size() != bindings.size()){
        return false;
    } else {
        //compare each of the bindings is the same. Bindings are sorted so they will match
        for (int i = 0; i < bindings.size(); i++) {
            if (other.bindings[i].binding != bindings[i].binding) {
                return false;
            }
            if (other.bindings[i].descriptorType != bindings[i].descriptorType) {
                return false;
            }
            if (other.bindings[i].descriptorCount != bindings[i].descriptorCount) {
                return false;
            }
            if (other.bindings[i].stageFlags != bindings[i].stageFlags) {
                return false;
            }
        }
        return true;
    }
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const{
    size_t seed = std::hash<size_t>()(bindings.size());

    for (const auto& b : bindings) {
        //pack the binding data
        hashCombine(seed, b.binding, static_cast<uint32_t>(b.descriptorType), b.descriptorCount, static_cast<uint32_t>(b.stageFlags));
    }

    return seed;
}

DescriptorBuilder::DescriptorBuilder(DescriptorLayoutCache& cache, DescriptorAllocator& allocator) : cache{cache}, allocator{allocator} {
}

DescriptorBuilder::~DescriptorBuilder() {
}

DescriptorBuilder& DescriptorBuilder::bindBuffer(uint32_t binding, vk::DescriptorBufferInfo& bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags) {
    //create the descriptor binding for the layout
    vk::DescriptorSetLayoutBinding newBinding{};
    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;
    newBinding.pImmutableSamplers = nullptr;
    newBinding.stageFlags = stageFlags;
    newBinding.binding = binding;

    bindings.push_back(newBinding);

    //create the descriptor write
    vk::WriteDescriptorSet newWrite{};
    newWrite.pNext = nullptr;
    newWrite.descriptorCount = 1;
    newWrite.descriptorType = type;
    newWrite.pBufferInfo = &bufferInfo;
    newWrite.dstBinding = binding;

    writes.push_back(newWrite);
    return *this;
}

DescriptorBuilder& DescriptorBuilder::bindImage(uint32_t binding, vk::DescriptorImageInfo& imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags) {
    //create the descriptor binding for the layout
    vk::DescriptorSetLayoutBinding newBinding{};
    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;
    newBinding.pImmutableSamplers = nullptr;
    newBinding.stageFlags = stageFlags;
    newBinding.binding = binding;

    bindings.push_back(newBinding);

    //create the descriptor write
    vk::WriteDescriptorSet newWrite{};
    newWrite.pNext = nullptr;
    newWrite.descriptorCount = 1;
    newWrite.descriptorType = type;
    newWrite.pImageInfo = &imageInfo;
    newWrite.dstBinding = binding;

    writes.push_back(newWrite);
    return *this;
}

bool DescriptorBuilder::build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout) {
    //build layout first
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.pNext = nullptr;
    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = bindings.size();

    layout = cache.createDescriptorLayout(layoutInfo);

    //allocate descriptor
    bool success = allocator.allocateDescriptor(layout, set);
    if (!success) return false;

    //write descriptor
    for (auto& w : writes) {
        w.dstSet = set;
    }

    allocator.updateDescriptor(writes);
    return true;
}

bool DescriptorBuilder::build(vk::DescriptorSet& set) {
    //build layout first
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.pNext = nullptr;
    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = bindings.size();

    auto layout = cache.createDescriptorLayout(layoutInfo);

    //allocate descriptor
    bool success = allocator.allocateDescriptor(layout, set);
    if (!success) return false;

    //write descriptor
    for (auto& w : writes) {
        w.dstSet = set;
    }

    allocator.updateDescriptor(writes);
    return true;
}*/