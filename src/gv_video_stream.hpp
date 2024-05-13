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

public:
    GVVideoStream();
    ~GVVideoStream();

    bool get_on_memory() const { return on_memory; }
    void set_on_memory(bool value) { on_memory = value; }

	Ref<VideoStreamPlayback> _instantiate_playback() override;
};
