#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class GVVideoStream : public VideoStream {
    GDCLASS(GVVideoStream, VideoStream);

protected:
	static void _bind_methods();

public:
    GVVideoStream();
    ~GVVideoStream();

	Ref<VideoStreamPlayback> _instantiate_playback() override;
};
