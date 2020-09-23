<img src="media/logo.png" width="600">
<a href='https://ci.ultralig.ht/job/Ultralight/job/master/'><img src='https://ci.ultralig.ht/buildStatus/icon?job=Ultralight%2Fmaster'></a>

[Website](https://ultralig.ht) | [Join our Discord!](https://chat.ultralig.ht) | [Forum](https://forum.ultralig.ht) | [Browse API](https://github.com/ultralight-ux/Ultralight-API/) | [Quick Start / Documentation](https://docs.ultralig.ht) | [Follow on Twitter](https://twitter.com/ultralight_ux)

## Discord Chat

Got a question about Ultralight? Come chat with us on Discord!

[<strong>Join the Ultralight Discord!</strong> <img src="media/discord-logo.svg">](https://chat.ultralig.ht)

# About

Ultralight is a lightweight, cross-platform, HTML rendering engine for desktop apps and games. It supports most modern HTML5 and JavaScript features while still remaining small in binary size and memory usage.

API is available for C and C++, with community bindings for C#, Rust, Java, and Go in our forum [here](https://forum.ultralig.ht/t/community-language-bindings).

## Why Would I Use This Over WebKit?

You can think of Ultralight like a smaller, lighter, cross-platform WebKit that's way more "hackable" and is designed to display HTML in an existing app rather than function as a standalone browser.

The API allows you to integrate your native code deeply with JavaScript (we offer bare-metal access to the JavaScriptCore VM API) and allows you to customize file system loaders, font loading, clipboard integration, and drawing itself (via GPU display lists or the Surface API).

### Base Memory Usage

|            | Ultralight  | Electron  |
|------------|-------------|-----------|
| Windows    | 9 MB        | 72 MB     |
| macOS      | 17 MB       | 97 MB     |
| Linux      | 15 MB       | 136 MB    |

### Base Distributable Size

|            | Ultralight  | Electron  |
|------------|-------------|-----------|
| Windows    | 30 MB       | 147 MB    |
| macOS      | 43 MB       | 163 MB    |
| Linux      | 49 MB       | 179 MB    |

### Comparison with Chromium

|                   | Ultralight               | Chromium                    |
|-------------------|--------------------------|-----------------------------|
| Renderer          | Proprietary (GPU)        | Skia (CPU) / GPU Compositor |
| Layout Engine     | WebCore (from WebKit)    | Blink                       |
| JavaScript Engine | JavaScriptCore           | V8                          |
| IPC               | None (Single Process)    | Yes (Multi Process)         |

# Architecture

Ultralight is a new port of WebKit combined with a new lightweight renderer intended specifically for embedding in apps and games.

**Our port of WebCore/JavaScriptCore is at: https://github.com/ultralight-ux/WebCore**

**Our cross-platform desktop app runtime is at: https://github.com/ultralight-ux/AppCore**

## Virtual GPU Renderer

Our optional, high-performance GPU renderer takes advantage of GPU hardware when available. All painting is performed via virtual display lists and translated to your target platform at runtime (see the GPUDriver interface). This renderer is enabled by default when using Ultralight via the AppCore runtime.

## Accelerated CPU Renderer

For easier integration into games and environments with limited GPU, Ultralight can also render to an offscreen pixel buffer using an SIMD-accelerated CPU rendering pipeline. Paths, gradients, and patterns in this mode are rasterized using [Blend2D's high-performance JIT compiler](https://blend2d.com).

## HTML Renderer For Games

Ultralight is designed to integrate well into existing games and game engines. Developers have complete control over asset loading via the FileSystem interface and have the ability to provide the library a custom, lockable pixel-buffer for each View to render into (eg, for display of HTML in a texture).

For more info about integrating Ultralight into a game, please see: https://docs.ultralig.ht/docs/integrating-with-games

## Cross-Platform AppCore Runtime

AppCore is an optional, cross-platform desktop app runtime built on top of Ultralight that handles window creation, native event loops, native graphics drivers (D3D11, D3D12, Metal, OpenGL), and more.

You can use it to build desktop HTML apps in a manner similar to Electron. It's currently under active development and only supports single-window apps at this writing. A number of planned features (app icons, automatic packager, borderless windows with custom draggable titlebars, etc) are missing but on the way.

For more info about using Ultralight in a desktop app, please see: https://docs.ultralig.ht/docs/writing-your-first-app

# Licensing

## Free License

Ultralight is __free__ for non-commercial use and __free__ for commercial use by companies with less than $100K gross annual revenue. For more info see [LICENSE.txt](license/LICENSE.txt)

## Commercial Licensing and Support 

We offer commercial licensing, priority support, and source code. To learn more,  please [visit our website](https://ultralig.ht).

# Getting the Latest SDK

You can get the latest SDK for each platform via the following links:

 * [Windows       | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-win-x64.7z)
 * [Windows (UWP) | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-win-uwp-x64.7z)
 * [Linux         | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-linux-x64.7z)
 * [macOS         | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-mac-x64.7z)

### Getting Pre-Built Binaries for a Specific Commit

Pre-built binares are uploaded to the following S3 buckets every commit:

 * __Archived Binaries__: <https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/>

Click any of the links for an XML listing of available files.

# Useful Links

| Link                       | URL                                                 |
| -------------------------- | --------------------------------------------------- |
| __Join our Discord!__      | <https://chat.ultralig.ht>                          |
| __Support Forum__          | <https://forum.ultralig.ht>                         |
| __Browse API Headers__     | <https://github.com/ultralight-ux/Ultralight-API/>  |
| __Docs / Getting Started__ | <https://docs.ultralig.ht>                          |
| __Follow Us on Twitter__   | <https://twitter.com/ultralight_ux>                 |
