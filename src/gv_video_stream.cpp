#include "gv_video_stream.hpp"
#include "gv_video_stream_playback.hpp"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GVVideoStream::_bind_methods()
{
	// ClassDB::bind_method(D_METHOD("hello_node"), &MyNode::hello_node);
}

GVVideoStream::GVVideoStream()
{
}

GVVideoStream::~GVVideoStream()
{
}

Ref<VideoStreamPlayback> GVVideoStream::_instantiate_playback()
{
    // return Ref<GVVideoStreamPlayback>();

    UtilityFunctions::print("GVVideoStream::_instantiate_playback()");

    Ref<FileAccess> fa = FileAccess::open(get_file(), FileAccess::READ);

    UtilityFunctions::print("GVVideoStream::_instantiate_playback file: " + get_file());

    if (!fa.is_valid()) {
        return Ref<VideoStreamPlayback>();
    }
    Ref<GVVideoStreamPlayback> pb;
    pb.instantiate();
    if (pb->load(fa) != OK) {
        return nullptr;
    }
    return pb;
}