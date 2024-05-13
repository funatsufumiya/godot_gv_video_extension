#include "gv_video_stream_playback.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GVVideoStreamPlayback::_bind_methods()
{
	// ClassDB::bind_method(D_METHOD("hello_node"), &MyNode::hello_node);
}

GVVideoStreamPlayback::GVVideoStreamPlayback()
{
}

GVVideoStreamPlayback::~GVVideoStreamPlayback()
{
}

Error GVVideoStreamPlayback::load(Ref<FileAccess> p_file_access)
{
    return OK;
}