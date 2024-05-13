#include "gv_video_stream.hpp"
#include "gv_video_stream_playback.hpp"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GVVideoStream::_bind_methods()
{
	// ClassDB::bind_method(D_METHOD("hello_node"), &MyNode::hello_node);
	ClassDB::bind_method(D_METHOD("get_on_memory"), &GVVideoStream::get_on_memory);
    ClassDB::bind_method(D_METHOD("set_on_memory", "value"), &GVVideoStream::set_on_memory);
    ClassDB::add_property(get_class_static(), PropertyInfo(Variant::BOOL, "on_memory"), "set_on_memory", "get_on_memory");
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

    // UtilityFunctions::print("GVVideoStream::_instantiate_playback()");

    Ref<FileAccess> fa = FileAccess::open(get_file(), FileAccess::READ);

    // UtilityFunctions::print("GVVideoStream::_instantiate_playback file: ", get_file(), ", on_memory: ", on_memory);

    if (!fa.is_valid()) {
        return nullptr;
    }
    Ref<GVVideoStreamPlayback> pb;
    pb.instantiate();
    if (pb->load(fa, on_memory) != OK) {
        return nullptr;
    }
    return pb;
}