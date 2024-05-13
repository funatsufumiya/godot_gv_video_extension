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
    if (texture.is_null()) {
        texture = ImageTexture::create_from_image(Image::create(1, 1, false, Image::FORMAT_RGBA8));
    }
}

GVVideoStreamPlayback::~GVVideoStreamPlayback()
{
}

Error GVVideoStreamPlayback::load(Ref<FileAccess> p_file_access, bool onMemory)
{
    reader.emplace(p_file_access, onMemory);
    // reader.emplace(std::make_optional<GpuVideoReader>(p_file_access, onMemory));
    return OK;
}

void GVVideoStreamPlayback::_stop() {
    // UtilityFunctions::print("GVVideoStreamPlayback::_stop()");
    playback_position = 0.0;
    is_playing = false;
    is_paused = false;
}

void GVVideoStreamPlayback::_play() {
    // UtilityFunctions::print("GVVideoStreamPlayback::_play()");
    is_playing = true;
    is_paused = false;
}

bool GVVideoStreamPlayback::_is_playing() const {
    return is_playing;
}

void GVVideoStreamPlayback::_set_paused(bool paused) {
    is_paused = paused;
}

bool GVVideoStreamPlayback::_is_paused() const {
    // UtilityFunctions::print("GVVideoStreamPlayback::_is_paused() is_paused: ", is_paused);
    return is_paused;
}

double GVVideoStreamPlayback::_get_length() const {
    return dummy_length;
}

double GVVideoStreamPlayback::_get_playback_position() const {
    return playback_position;
}

void GVVideoStreamPlayback::_seek(double time) {
    UtilityFunctions::print("GVVideoStreamPlayback::_seek() time: ", time);
    playback_position = time;
}

void GVVideoStreamPlayback::_set_audio_track(int32_t idx) {
    // do nothing
    UtilityFunctions::push_warning("GVVideoStreamPlayback::_set_audio_track() do nothing");
}

Ref<Texture2D> GVVideoStreamPlayback::_get_texture() const {
    return texture;
}

void GVVideoStreamPlayback::_update(double delta) {
    if (is_playing && !is_paused) {
        playback_position += delta;
        if (playback_position >= dummy_length) {
            playback_position = dummy_length;
            _stop();
        }
    }

    if (reader.has_value()) {
        // UtilityFunctions::print("GVVideoStreamPlayback::_update() playback_position: ", playback_position);

        PackedByteArray buffer = reader->read_at_time(playback_position);

        // UtilityFunctions::print("GVVideoStreamPlayback::_update() buffer.size(): ", buffer.size());
        // UtilityFunctions::print("GVVideoStreamPlayback::_update() reader->getWidth(): ", reader->getWidth());
        // UtilityFunctions::print("GVVideoStreamPlayback::_update() reader->getHeight(): ", reader->getHeight());
        // UtilityFunctions::print("GVVideoStreamPlayback::_update() reader->getGodotImageFormat(): ", reader->getGodotImageFormat());

        // update image
        if (image.is_null()) {
            // UtilityFunctions::print("GVVideoStreamPlayback::_update() image is null");
            
            // create dummy buffer for test
            // PackedByteArray dummy_buffer;
            // dummy_buffer.resize(reader->getWidth() * reader->getHeight() * 4);

            image = Image::create_from_data(
                reader->getWidth(),
                reader->getHeight(),
                false,
                reader->getGodotImageFormat(),
                buffer
                // Image::FORMAT_RGBA8,
                // dummy_buffer
            );
        }else {
            // UtilityFunctions::print("GVVideoStreamPlayback::_update() image is not null");
            image->set_data(
                reader->getWidth(),
                reader->getHeight(),
                false,
                reader->getGodotImageFormat(),
                buffer
            );
        }

        // UtilityFunctions::print("GVVideoStreamPlayback::_update() image->get_width(): ", image->get_width());
        // UtilityFunctions::print("GVVideoStreamPlayback::_update() image->get_height(): ", image->get_height());

        // update texture
        if (texture.is_null()) {
            // UtilityFunctions::print("GVVideoStreamPlayback::_update() texture is null");
            texture = ImageTexture::create_from_image(image);
        }else if (!texture.is_valid()
            || texture->get_width() != reader->getWidth()
            || texture->get_height() != reader->getHeight()
            || texture->get_format() != reader->getGodotImageFormat()
            )
        {
            texture->set_image(image);
        }else{
            texture->update(image);
        }
    }
    

    // UtilityFunctions::print("GVVideoStreamPlayback::_update() playback_position: ", playback_position);
}

int32_t GVVideoStreamPlayback::_get_channels() const {
    return 0;
}

int32_t GVVideoStreamPlayback::_get_mix_rate() const {
    return 0;
}