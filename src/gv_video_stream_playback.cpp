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
    use_parallel_update = false;
    stop_thread = false;
}

GVVideoStreamPlayback::~GVVideoStreamPlayback()
{
    stop_thread = true;
    buffer_cv.notify_all();
    if (decode_thread.joinable()) decode_thread.join();
}

Error GVVideoStreamPlayback::load(Ref<FileAccess> p_file_access, bool onMemory, bool pauseAtEnd)
{
    pause_at_end = pauseAtEnd;
    reader.emplace(p_file_access, onMemory);

    if (use_parallel_update && reader.has_value()) {
        stop_thread = false;
        if (decode_thread.joinable()) {
            stop_thread = true;
            buffer_cv.notify_all();
            decode_thread.join();
            stop_thread = false;
        }
        buffer_ready = false;
        requested_position = -1.0;
        decode_thread = std::thread(&GVVideoStreamPlayback::decode_worker, this);
    }
    return OK;
}

void GVVideoStreamPlayback::_stop() {
    // UtilityFunctions::print("GVVideoStreamPlayback::_stop()");
    playback_position = 0.0;
    is_playing = false;
    is_paused = false;
    // 並列スレッドはここでは止めない（load/unloadで管理）
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
    if (reader.has_value()) {
        return reader->getFrameCount() / reader->getFramePerSecond();
    }else{
        return 0.0;
    }
}

double GVVideoStreamPlayback::_get_playback_position() const {
    return playback_position;
}

void GVVideoStreamPlayback::_seek(double time) {
    // UtilityFunctions::print("GVVideoStreamPlayback::_seek() time: ", time);
    playback_position = time;
}

void GVVideoStreamPlayback::_set_audio_track(int32_t idx) {
    // do nothing
    // UtilityFunctions::push_warning("GVVideoStreamPlayback::_set_audio_track() do nothing");
}

Ref<Texture2D> GVVideoStreamPlayback::_get_texture() const {
    return texture;
}

void GVVideoStreamPlayback::_update(double delta) {
    if (is_playing && !is_paused) {
        playback_position += delta;
        if (playback_position >= _get_length()) {
            playback_position = _get_length();
            if (pause_at_end) {
                _stop();
                playback_position = _get_length();
                _set_paused(true);
            } else {
                _stop();
            }
        }
    }

    if (!reader.has_value()) return;

    if (use_parallel_update) {
        bool need_new_frame = false;
        {
            std::lock_guard<std::mutex> lock(buffer_mutex);
            if (!buffer_ready && requested_position != playback_position) {
                requested_position = playback_position;
                need_new_frame = true;
            }
        }
        if (need_new_frame) {
            buffer_cv.notify_all();
        }
        PackedByteArray buffer;
        bool got_new_frame = false;
        {
            std::unique_lock<std::mutex> lock(buffer_mutex);
            if (buffer_ready) {
                buffer = decoded_buffer;
                buffer_ready = false;
                got_new_frame = true;
            }
        }
        if (!got_new_frame && !decoded_buffer.is_empty()) {
            std::lock_guard<std::mutex> lock(buffer_mutex);
            buffer = decoded_buffer;
        }
        if (buffer.is_empty()) return;
        if (image.is_null()) {
            image = Image::create_from_data(
                reader->getWidth(),
                reader->getHeight(),
                false,
                reader->getGodotImageFormat(),
                buffer
            );
        } else {
            image->set_data(
                reader->getWidth(),
                reader->getHeight(),
                false,
                reader->getGodotImageFormat(),
                buffer
            );
        }
        if (texture.is_null()) {
            texture = ImageTexture::create_from_image(image);
        } else if (!texture.is_valid()
            || texture->get_width() != reader->getWidth()
            || texture->get_height() != reader->getHeight()
            || texture->get_format() != reader->getGodotImageFormat()) {
            texture->set_image(image);
        } else {
            texture->update(image);
        }
    } else {
        PackedByteArray buffer = reader->read_at_time(playback_position);
        if (image.is_null()) {
            image = Image::create_from_data(
                reader->getWidth(),
                reader->getHeight(),
                false,
                reader->getGodotImageFormat(),
                buffer
            );
        } else {
            image->set_data(
                reader->getWidth(),
                reader->getHeight(),
                false,
                reader->getGodotImageFormat(),
                buffer
            );
        }
        if (texture.is_null()) {
            texture = ImageTexture::create_from_image(image);
        } else if (!texture.is_valid()
            || texture->get_width() != reader->getWidth()
            || texture->get_height() != reader->getHeight()
            || texture->get_format() != reader->getGodotImageFormat()) {
            texture->set_image(image);
        } else {
            texture->update(image);
        }
    }
}

void GVVideoStreamPlayback::decode_worker() {
    double last_position = -1.0;
    while (!stop_thread) {
        double pos = -1.0;
        {
            std::unique_lock<std::mutex> lock(buffer_mutex);
            buffer_cv.wait(lock, [this, &last_position]{
                return stop_thread || (requested_position != last_position && !buffer_ready);
            });
            if (stop_thread) break;
            pos = requested_position;
        }
        if (reader.has_value() && pos >= 0.0) {
            PackedByteArray buf = reader->read_at_time(pos);
            {
                std::lock_guard<std::mutex> lock(buffer_mutex);
                decoded_buffer = buf;
                buffer_ready = true;
                last_position = pos;
            }
            buffer_cv.notify_all();
        }
    }
}

int32_t GVVideoStreamPlayback::_get_channels() const {
    return 0;
}

int32_t GVVideoStreamPlayback::_get_mix_rate() const {
    return 0;
}