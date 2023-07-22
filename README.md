<img src="media/logo.png" width="200">
<a href='https://ci.ultralig.ht/job/Ultralight/job/master/'><img src='https://ci.ultralig.ht/buildStatus/icon?job=Ultralight%2Fmaster'></a>

[Website](https://ultralig.ht) | [Chat on Discord!](https://chat.ultralig.ht) | [Browse C++ API](https://ultralig.ht/api/cpp/1_3_0/) | [Browse C API](https://ultralig.ht/api/c/1_3_0/) | [Support Docs](https://docs.ultralig.ht) | [Twitter](https://twitter.com/ultralight_ux)

## :video_game: Discord Chat

Got a question about Ultralight? [<strong>Chat with us on Discord!</strong> <img src="media/discord-logo.svg">](https://chat.ultralig.ht)

# :star: About

Ultralight makes it easy for C/C++ developers to seamlessly integrate web-content into games and desktop apps.

Use our lightweight, high-performance renderer to display HTML/JS/CSS on a variety of platforms and hardware targets.

## Key Features

<dl>
<dt>1. :zap: Modern HTML/JS/CSS Support:</dt>
<dd>
    Based on WebKit, Ultralight enjoys many of the same cutting-edge web features as Apple's Safari browser, simplifying development and reducing production costs.
</dd>

<dt>2. :sparkles: Powerful and Lightweight:</dt>
<dd>
    Thanks to its highly-customizable CPU and GPU renderers, Ultralight offers developers unprecedented control over performance and memory usage-- making it the go-to choice for users who demand speed in constrained environments.
</dd>

<dt>3. :jigsaw: Cross-Platform Compatibility:</dt>
<dd>
    With support for Windows, macOS, Linux, Xbox, PS4, and PS5 (and ARM64 coming in 1.4), Ultralight ensures your content displays smoothly and consistently on a wide range of platforms and hardware targets.
</dd>

<dt>4. :repeat: Deep C/C++ and JavaScript Integration:</dt>
<dd>
    Ultralight facilitates seamless C/C++ and JavaScript integration through direct access to the JavaScriptCore API. Expose native state to web pages, extend JavaScript classes, define C/C++ callbacks, and more.
</dd>

<dt>5. :wrench: Low-Level Customization:</dt>
<dd>
    Ultralight allows developers the ability to override platform-specific functionality-- granting control over such things as file-system access, clipboard management, font loading, and more to suit the unique needs of each target environment.
</dd>
</dl>

## :space_invader: For Game Developers

Developed in collaboration with top AAA game studios, Ultralight is engineered to display high-performance web-content across a diverse set of hardware targets and game engine environments.

Choose between our CPU renderer for fast, reliable rendering or our customizable GPU renderer for unparalleled animation performance.

__→ Learn more by visiting our [Game Integration Guide](https://docs.ultralig.ht/docs/integrating-with-games).__

## :desktop_computer: For Desktop-App Developers

Get the best of both worlds-- save time by building your front-end with HTML/CSS/JS while retaining the power and performance of C/C++.

Embed the renderer directly within your existing run loop or leverage our AppCore framework to do all the heavy lifting for you, offering convenient native window management and accelerated rendering.

__→ Learn more by visiting our [Desktop App Guide](https://docs.ultralig.ht/docs/writing-your-first-app).__

## :classical_building: Architecture

Ultralight is a platform-agnostic port of WebKit for games and desktop apps.

Portions of the library are open-source, here's how the library is laid out:

|                    | Description                              | License     | Source Code              |
|--------------------|------------------------------------------|-------------|--------------------------|
| __UltralightCore__ | Low-level graphics renderer              | Proprietary | (Available with license) |
| __WebCore__        | HTML layout engine (WebKit fork)         | LGPL/BSD    | [ultralight-ux/WebCore](https://github.com/ultralight-ux/WebCore) |
| __Ultralight__     | View, compositor, and event logic        | Proprietary | (Avaiable with license)  |
| __AppCore__        | :sparkles: Optional, desktop app runtime | LGPL        | [ultralight-ux/AppCore](https://github.com/ultralight-ux/AppCore) |

### :rocket: Dual High-Performance Renderers

We offer two different renderers for integration into a variety of target environments.

<table style="width: 100%;">
<tr><th> :sparkle: CPU Renderer</th><th> :eight_spoked_asterisk: GPU Renderer</th></tr>
<tr>
<td style="width: 50%;">

* Fast, lightweight, and easy to integrate.
* Renders to a 32-bit BGRA pixel buffer.
* Based on Skia, accelerated via multi-threaded SIMD.
* See the [Game Integration Guide](https://docs.ultralig.ht/docs/integrating-with-games) for more info.

</td>
<td style="width: 50%;">

* High-performance, customizable, and portable.
* Renders on the GPU to a texture.
* Accelerated via tesselated geometry and pixel shaders.
* See [Using a Custom GPUDriver](https://docs.ultralig.ht/docs/using-a-custom-gpudriver) for more info.

</td></tr>
</table>

## :lock: Licensing

It's easy to license Ultralight in projects big and small.

<table style="width: 100%;">
<tr><th> :heart_eyes: Free License</th><th> :crown: Commercial License</th></tr>
<tr>
<td style="width: 50%;">

 * __Free__ for non-commercial use.
 * __Free__ for commercial use by indie companies (< $100K annual revenue).
 * Full SDK available for all users, download and try now.
 * See [LICENSE.txt](license/LICENSE.txt) for more info.

</td>
<td style="width: 50%;">

* Paid licenses for commercial use.
* Enterprise licensing, custom contracts, and volume discounts available.
* Performance tools, extra configurations, and source code.
* See [our website](https://ultralig.ht) for more info.

</td></tr>
</table>

## :eyes: Getting the Latest SDK

You can get the latest SDK for each platform below (contact us for Xbox, PS4, and PS5):

 * [Windows       | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-win-x64.7z)
 * [Linux         | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-linux-x64.7z)
 * [macOS         | x64](https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-mac-x64.7z)

### Getting Archived Binaries for a Specific Commit

Pre-built binares are uploaded to the following S3 buckets every commit:

 * __Archived Binaries__: <https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/>

## :hammer_and_wrench: Building the Samples

To build the samples in this repo, please [follow the instructions here](https://docs.ultralig.ht/docs/trying-the-samples).


## :information_source: Useful Links

| Link                       | URL                                                 |
| -------------------------- | --------------------------------------------------- |
| __Join our Discord!__      | <https://chat.ultralig.ht>                          |
| __Docs / Getting Started__ | <https://docs.ultralig.ht>                          |
| __Browse C++ API__         | <https://ultralig.ht/api/cpp/1_3_0/>                |
| __Browse C API__           | <https://ultralig.ht/api/c/1_3_0/>                  |
| __Follow Us on Twitter__   | <https://twitter.com/ultralight_ux>                 |
