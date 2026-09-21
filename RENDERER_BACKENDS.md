# Renderer backends: Vulkan vs OpenGL

Short answer: **there is no working OpenGL path any more.** The viewer is Vulkan
only, and that is not something a cmake flag or a runtime switch can change. This
file records what is actually left of the GL backend and what it would take to
revive it, so the question does not have to be re-investigated each time.

Checked against upstream `Deamon87/WebWowViewerCpp` as merged on 2026-09-21.

## Is it toggleable?

No. Neither at runtime nor at build time:

- **Runtime.** The backend is picked by a string passed to
  `IDeviceFactory::createDevice(gapiName, ...)`, which still understands
  `"ogl2"`, `"ogl3"`, `"ogl4"` and `"vulkan"`. But `src/main.cpp` hardcodes
  `std::string rendererName = "vulkan";` with the GL lines commented out just
  above it. There is no command line argument and no config field for it.
- **Build time.** `wowViewerLib/CMakeLists.txt` sets `LINK_OGL2`, `LINK_OGL3`
  and `LINK_OGL4` to `0` with plain `set()`, not `option()`, so they cannot even
  be overridden with `-DLINK_OGL3=1`; the `set()` shadows any cache entry. Only
  `LINK_VULKAN` is a real `option()` (default `ON`), and turning it *off* does
  not fall back to GL, it just leaves the viewer with no usable backend at all.

## Why flipping the flags is not enough

Even with `LINK_OGL3` forced to `1` in the CMakeLists, the app cannot draw a
frame. Three separate layers are missing:

1. **No GL renderer.** The renderer layer added above `gapi/` handles exactly one
   device type. `MapSceneRendererFactory::createForwardRenderer` switches on
   `device->getDeviceType()`, implements only `GDeviceType::GVulkan`, and returns
   `nullptr` for everything else. `FrontendUIRendererFactory` is the same. Both
   `renderer/mapScene/` and `src/ui/renderer/uiScene/` contain only a `vulkan/`
   subdirectory.
2. **No GLSL shaders.** `wowViewerLib/shaders/` now contains only `slang/`; the
   `glsl/` source tree is gone. In `wowViewerLib/shaders/CMakeLists.txt` the
   `configure_filesVLK(... "glsl" ...)` call is commented out and `GLSL20Files` /
   `GLSL30Files` are never assigned, so no `glsl/` directory is produced in the
   build dir. `GDeviceGL33` still tries to read `./glsl/glsl3.3/<name>` at
   runtime and would find nothing.
3. **Dead device classes.** `gapi/ogl2.0/`, `gapi/ogl3.3/` and `gapi/ogl4.x/`
   still exist and still compile when their `LINK_OGL*` macro is defined, but
   nothing above them can use them. `gapi/ogl4.x` never had a working device:
   `IDeviceFactory` has its construction commented out.

So reviving GL means porting the map scene and UI renderers to it and bringing
back a GLSL shader pipeline, not setting a flag. That is upstream work.

## What this means for the build here

- `LINK_VULKAN` should stay `ON`. This fork deliberately uses the upstream
  default; the earlier local change that set it to `OFF` was dropped during the
  upstream merge.
- The Vulkan SDK is a hard requirement, including `slangc` for compiling the
  slang shaders into spirv. cmake looks for it in `$VULKAN_SDK/bin` and fails
  configuration outright if it is missing; override with `-DSLANG_COMPILER=...`.
- `LINK_GLEW` is still `1` on MSVC and `glew32.lib` is still linked, which is why
  `glew32.dll` is copied next to the executable under `USE_CUSTOM_CHANGES`. With
  every `LINK_OGL*` at `0` nothing actually calls into GLEW, and `glew32.dll`
  does not appear in the executable's import table. The copy is kept because it
  costs nothing and would be needed again if a GL backend ever returns.

## If you want to check whether this is still true

```
# is any non-vulkan renderer implemented?
ls wowViewerLib/src/renderer/mapScene/
grep -n "GDeviceType" wowViewerLib/src/renderer/mapScene/MapSceneRendererFactory.cpp

# are glsl shader sources back?
ls wowViewerLib/shaders/

# is the backend still hardcoded?
grep -n "rendererName" src/main.cpp
```
