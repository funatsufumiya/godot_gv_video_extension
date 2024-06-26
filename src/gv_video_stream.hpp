#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class GVVideoStream : public VideoStream {
    GDCLASS(GVVideoStream, VideoStream);

protected:
	static void _bind_methods();
    bool on_memory = false;
    bool pause_at_end = false;

public:
    GVVideoStream();
    ~GVVideoStream();

    bool get_on_memory() const { return on_memory; }
    void set_on_memory(bool value) { on_memory = value; }
    bool get_pause_at_end() const { return pause_at_end; }
    void set_pause_at_end(bool value) { pause_at_end = value; }

	Ref<VideoStreamPlayback> _instantiate_playback() override;
};
