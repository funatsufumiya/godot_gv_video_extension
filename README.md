# godot_gv_video_extension

[gv-video](https://github.com/Ushio/ofxExtremeGpuVideo#binary-file-format-gv) extension for Godot Engine.

This enables **alpha chanel (opacity)** video, and fast video decoding using compressed texture format like DXT1, DXT3, DXT5.

- Tested on Godot 4.2, on Windows/macOS. probably also works on Linux.

## Binary file format (gv)

```text
0: uint32_t width
4: uint32_t height
8: uint32_t frame count
12: float fps
16: uint32_t format (DXT1 = 1, DXT3 = 3, DXT5 = 5, BC7 = 7)
20: uint32_t frame bytes
24: raw frame storage (lz4 compressed)
eof - (frame count) * 16: [(uint64_t, uint64_t)..<frame count] (address, size) of lz4, address is zero based from file head
```

## Install

see [Releases](https://github.com/funatsufumiya/godot_gv_video_extension/releases)

(NOTE: if you need as `git submodule`, use [godot_gv_video_extension_bin](https://github.com/funatsufumiya/godot_gv_video_extension_bin) repository)

## Usage

- Unzip `gv_video.zip` on [Releases](https://github.com/funatsufumiya/godot_gv_video_extension/releases) and put `gv_video` into `addons` folder of your Godot project.
- Open your project in Godot
- Create a `VideoStreamPlayer` node
- Set `Stream` property to `GVVideoStream`
- Set `file` property of `GVVideoStream` to a `res://` path of a `.gv` file


## Build and Run

- `git submodule update --init --recursive --recommend-shallow --depth 1`
- `scons`
- `scons target=template_release`
- `godot project/project.godot` (only first time)
- `godot --path project/`
