//
//  GpuVideoReader.cpp
//  emptyExample
//
//  Created by Nakazi_w0w on 3/29/16.
//
//

#include "gv_video_reader.hpp"
#include <cassert>
#include <algorithm>
#include "lz4.h"

GpuVideoReader::GpuVideoReader(Ref<FileAccess> io, bool onMemory) {
    _onMemory = onMemory;
    
	_io = io;
    
	// _io->seek_end(0);
    _rawSize = _io->get_length();
	_io->seek(0);

// #define R(v) if(_io->read(&v, sizeof(v)) != sizeof(v)) { assert(0); }
    // R(_width);
    // R(_height);
    // R(_frameCount);
    // R(_framePerSecond);
    // R(_format);
    // R(_frameBytes);
// #undef R

    _width = _io->get_32();
    _height = _io->get_32();
    _frameCount = _io->get_32();
    _framePerSecond = _io->get_float();
    _format = static_cast<GPU_COMPRESS>(_io->get_32());
    _frameBytes = _io->get_32();
    
    // read address
    _lz4Blocks.resize(_frameCount);
    size_t s = sizeof(Lz4Block);
    
	_io->seek(_rawSize - sizeof(Lz4Block) * _frameCount);
    // if(_io->read(_lz4Blocks.data(), sizeof(Lz4Block) * _frameCount) != sizeof(Lz4Block) * _frameCount) {
    PackedByteArray buf = _io->get_buffer(sizeof(Lz4Block) * _frameCount);
    if(buf.size() != sizeof(Lz4Block) * _frameCount) {
        assert(0);
    }
    
    // read all if needed
    if(_onMemory) {
        _memory.resize(_rawSize);
		_io->seek(0);
        // if(_io->read(_memory.data(), _rawSize) != _rawSize) {
        //     assert(0);
        // }
        if(_io->get_buffer(_rawSize).size() != _rawSize) {
            assert(0);
        }
		// _io.reset();
    } else {
		_io->seek(kRawMemoryAt);
        
        uint64_t buffer_size = 0;
        for(auto b : _lz4Blocks) {
            buffer_size = std::max(buffer_size, b.size);
        }
        
        _lz4Buffer.resize(buffer_size);
    }
}
GpuVideoReader::~GpuVideoReader() {

}
PackedByteArray GpuVideoReader::read(int frame) {
    assert(0 <= frame && frame < _lz4Blocks.size());
    Lz4Block lz4block = _lz4Blocks[frame];
    PackedByteArray out_buf;
    if(_onMemory) {
        out_buf.resize(_frameBytes);
        LZ4_decompress_safe((const char *)_memory.data() + lz4block.address, (char *)out_buf.ptr(), static_cast<int>(lz4block.size), _frameBytes);
    } else {
		_io->seek(lz4block.address);
        // if(_io->read(_lz4Buffer.data(), lz4block.size) != lz4block.size) {
        //     assert(0);
        // }
        PackedByteArray buf = _io->get_buffer(lz4block.size);
        out_buf.resize(_frameBytes);
        // LZ4_decompress_safe((const char *)_lz4Buffer.data(), (char *)dst, static_cast<int>(lz4block.size), _frameBytes);
        LZ4_decompress_safe((const char *)buf.ptr(), (char *)out_buf.ptr(), static_cast<int>(lz4block.size), _frameBytes);
    }
    return out_buf;
}

PackedByteArray GpuVideoReader::read_at_time(float time) {
    int frame = static_cast<int>(time * _framePerSecond);
    frame = std::max(0, std::min(static_cast<int>(_lz4Blocks.size()) - 1, frame));
    return read(frame);
}

godot::Image::Format GpuVideoReader::getGodotImageFormat() const {
    switch(_format) {
        case GPU_COMPRESS_DXT1:
            return Image::Format::FORMAT_DXT1;
        case GPU_COMPRESS_DXT3:
            return Image::Format::FORMAT_DXT3;
        case GPU_COMPRESS_DXT5:
            return Image::Format::FORMAT_DXT5;
        case GPU_COMPRESS_BC7:
            return Image::Format::FORMAT_BPTC_RGBA;
    }
}