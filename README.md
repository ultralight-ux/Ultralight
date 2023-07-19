<img src="media/logo.png" width="200">
<a href='https://ci.ultralig.ht/job/Ultralight/job/master/'><img src='https://ci.ultralig.ht/buildStatus/icon?job=Ultralight%2Fmaster'></a>

[Website](https://ultralig.ht) | [Join our Discord!](https://chat.ultralig.ht) | [C++ API](https://ultralig.ht/api/cpp/1_3_0/) | [C API](https://ultralig.ht/api/c/1_3_0/) | [Support Docs](https://docs.ultralig.ht) | [Twitter](https://twitter.com/ultralight_ux)

## Discord Chat

Got a question about Ultralight? Come chat with us on Discord!

[<strong>Join the Ultralight Discord!</strong> <img src="media/discord-logo.svg">](https://chat.ultralig.ht)

# About

Ultralight allows C/C++ developers to seamlessly integrate web-content into games and desktop apps. Our ultra-portable engine supports the latest HTML/CSS/JS standards while remaining exceptionally lightweight in binary size and memory usage.

## Key Features

<dl>
<dt>1. Lightweight and Powerful:</dt>
<dd>
    Thanks to its dual, high-performance CPU and GPU renderers, Ultralight offers the perfect balance between performance and resource consumption-- making it the go-to choice for developers who demand speed in constrained environments.
</dd>

<dt>2. Cross-Platform Compatibility:</dt>
<dd>
    With support for Windows, macOS, Linux, Xbox, PS4, and PS5 (and ARM64 coming in 1.4), Ultralight ensures your content displays smoothly and consistently on a wide range of platforms and hardware targets.
</dd>

<dt>3. Modern HTML/JS/CSS Support:</dt>
<dd>
    Based on WebKit, Ultralight enjoys many of the same cutting-edge web features as Apple's Safari browser, simplifying testing and reducing production costs.
</dd>

<dt>4. Deep C/C++ and JavaScript Integration:</dt>
<dd>
    Ultralight facilitates seamless C/C++ and JavaScript integration through direct access to the JavaScriptCore API. Expose native state to web pages, extend JavaScript classes, define C/C++ callbacks, and more.
</dd>

<dt>5. Low-Level Customization:</dt>
<dd>
    Ultralight allows you to override platform-specific functionality, granting OS-level control over file-system access, clipboard management, font loading, and more to suit the unique needs of your application.
</dd>
</dl>

## For Game Developers

Developed in collaboration with top AAA game studios, Ultralight is engineered to display high-performance web-content across a diverse set of hardware targets and game engine environments.

Choose between our CPU renderer for fast, reliable rendering or our customizable GPU renderer for unparalleled animation performance.

__→ Learn more by visiting our [Game Integration Guide](https://docs.ultralig.ht/docs/integrating-with-games).__

## For Desktop-App Developers

Get the best of both worlds-- save time by building your front-end with HTML/CSS/JS while retaining the power and performance of C/C++.

Embed the renderer directly within your existing run loop or leverage our AppCore framework to do all the heavy lifting for you, offering convenient native window management and accelerated rendering.

__→ Learn more by visiting our [Desktop App Guide](https://docs.ultralig.ht/docs/writing-your-first-app).__

# Architecture

Ultralight is a platform-agnostic port of WebKit for games and desktop apps.

Portions of the library are open-source, here's how the library is laid out:

|                    | Description                       | License     | Source Code              |
|--------------------|-----------------------------------|-------------|--------------------------|
| __UltralightCore__ | Low-level graphics renderer       | Proprietary | (Available with license) |
| __WebCore__        | HTML layout engine (WebKit fork)  | LGPL/BSD    | [ultralight-ux/WebCore](https://github.com/ultralight-ux/WebCore) |
| __Ultralight__     | View, compositor, and event logic | Proprietary | (Avaiable with license)  |
| __AppCore__        | Optional desktop app runtime      | LGPL        | [ultralight-ux/AppCore](https://github.com/ultralight-ux/AppCore) |

## Dual High-Performance Renderers

We support two different renderers for integration into a variety of target environments.

### CPU Renderer

For easy integration into games and other complex environments, Ultralight can render to an offscreen pixel buffer via a multi-threaded CPU rendering pipeline. For more info, see the [Game Integration Guide](https://docs.ultralig.ht/docs/integrating-with-games).

The library can also render directly to a user-supplied block of memory via the Surface API (see [Using a Custom Surface](https://docs.ultralig.ht/docs/using-a-custom-surface)).

Paths, gradients, images, and patterns in this pipeline are rasterized and composited in parallel using [Skia](https://skia.org/).

### GPU Renderer

Our optional, high-performance GPU renderer can render HTML directly on the GPU. All painting is performed via virtual display lists and translated to your target graphics platform at runtime. For more info, see [Using a Custom GPUDriver](https://docs.ultralig.ht/docs/using-a-custom-gpudriver).

The GPU renderer is enabled by default when using Ultralight via the AppCore runtime.

# Licensing

## Free License

Ultralight is __free__ for non-commercial use and __free__ for commercial use by companies with less than $100K gross annual revenue. For more info see [LICENSE.txt](license/LICENSE.txt)

## Commercial Licensing and Support 

We offer commercial licensing, priority support, and source code. To learn more,  please [visit our website](https://ultralig.ht).

# Building the Samples

To build the samples in this repo, please [follow the instructions here](https://docs.ultralig.ht/docs/trying-the-samples).

# Getting the Latest SDK

You can get the latest SDK for each platform via the following links:

 * [Windows       | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-win-x64.7z)
 * [Linux         | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-linux-x64.7z)
 * [macOS         | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-mac-x64.7z)

### Getting Archived Binaries for a Specific Commit

Pre-built binares are uploaded to the following S3 buckets every commit:

 * __Archived Binaries__: <https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/>

# Useful Links

| Link                       | URL                                                 |
| -------------------------- | --------------------------------------------------- |
| __Join our Discord!__      | <https://chat.ultralig.ht>                          |
| __Docs / Getting Started__ | <https://docs.ultralig.ht>                          |
| __Browse C++ API__         | <https://ultralig.ht/api/cpp/1_3_0/>                |
| __Browse C API__           | <https://ultralig.ht/api/c/1_3_0/>                  |
| __Follow Us on Twitter__   | <https://twitter.com/ultralight_ux>                 |
