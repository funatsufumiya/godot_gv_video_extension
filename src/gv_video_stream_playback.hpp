#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class GVVideoStreamPlayback : public VideoStreamPlayback {
    GDCLASS(GVVideoStreamPlayback, VideoStreamPlayback);

protected:
	static void _bind_methods();

public:
    GVVideoStreamPlayback();
    ~GVVideoStreamPlayback();

    Error load(Ref<FileAccess> p_file_access);
};
