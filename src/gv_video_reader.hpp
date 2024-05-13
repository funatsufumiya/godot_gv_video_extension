//
//  GpuVideoReader.hpp
//  emptyExample
//
//  Created by Nakazi_w0w on 3/29/16.
//
//

#pragma once
#include <cstdlib>
#include <vector>
#include <memory>
#include "gv_video.hpp"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>

using namespace godot;

class IGpuVideoReader {
public:
    virtual ~IGpuVideoReader() {}
    
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual uint32_t getFrameCount() const = 0;
    virtual float getFramePerSecond() const = 0;
    virtual GPU_COMPRESS getFormat() const = 0;
    virtual godot::Image::Format getGodotImageFormat() const = 0;
    virtual uint32_t getFrameBytes() const = 0;
    
    virtual bool isThreadSafe() const = 0;
    
    // virtual void read(uint8_t *dst, int frame) const = 0;
    virtual PackedByteArray read(int frame) = 0;
    virtual PackedByteArray read_at_time(float time) = 0;
};

class GpuVideoReader : public IGpuVideoReader {
public:
    GpuVideoReader(Ref<FileAccess> io, bool onMemory);
    ~GpuVideoReader();
    
    GpuVideoReader(const GpuVideoReader&) = delete;
    void operator=(const GpuVideoReader&) = delete;
    
    uint32_t getWidth() const override { return _width; }
    uint32_t getHeight() const override { return _height; }
    uint32_t getFrameCount() const override { return _frameCount; }
    float getFramePerSecond() const override { return _framePerSecond; }
    GPU_COMPRESS getFormat() const override { return _format; }
    godot::Image::Format getGodotImageFormat() const override;
    
    uint32_t getFrameBytes() const override { return _frameBytes; }
    
    bool isThreadSafe() const override { return _onMemory; }
    
    PackedByteArray read(int frame) override;
    PackedByteArray read_at_time(float time) override;
private:
    bool _onMemory = false;
    
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _frameCount = 0;
    float _framePerSecond = 0;
    GPU_COMPRESS _format = GPU_COMPRESS_DXT1;
    uint32_t _frameBytes = 0;
    std::vector<Lz4Block> _lz4Blocks;
    
	Ref<FileAccess> _io;
    std::vector<uint8_t> _memory;
    mutable std::vector<uint8_t> _lz4Buffer;
    
    uint64_t _rawSize = 0;
};