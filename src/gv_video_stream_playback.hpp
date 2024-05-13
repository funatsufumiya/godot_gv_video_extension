#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "gv_video_reader.hpp"

using namespace godot;

class GVVideoStreamPlayback : public VideoStreamPlayback {
    GDCLASS(GVVideoStreamPlayback, VideoStreamPlayback);

protected:
	static void _bind_methods();
    bool is_playing = false;
    bool is_paused = false;
    double playback_position = 0.0;
    double dummy_length = 5.0;
    Ref<ImageTexture> texture;
    Ref<Image> image;
    std::optional<GpuVideoReader> reader;

public:
    GVVideoStreamPlayback();
    ~GVVideoStreamPlayback();

    Error load(Ref<FileAccess> p_file_access, bool onMemory);

    void _stop() override;
    void _play() override;
    bool _is_playing() const override;
    void _set_paused(bool paused) override;
    bool _is_paused() const override;
    double _get_length() const override;
    double _get_playback_position() const override;
    void _seek(double time) override;
    void _set_audio_track(int32_t idx) override;
    Ref<Texture2D> _get_texture() const override;
    void _update(double delta) override;
    int32_t _get_channels() const override;
    int32_t _get_mix_rate() const override;
};
