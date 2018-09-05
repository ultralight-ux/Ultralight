# Ultralight

__Welcome to the Ultralight Universal SDK!__

This package contains everything you need to start building cross-platform HTML apps.

## Useful Links

| Link              | URL                                 |
| ----------------- | ----------------------------------- |
| __Slack Channel__ | <https://chat.ultralig.ht>          |
| __Support Forum__ | <https://discuss.ultralig.ht>       |
| __Twitter__       | <https://twitter.com/ultralight-ux> |


# Table of Contents

 - [Getting Started](#getting-started)
 	- [Build Requirements]()
 		- [Windows Build Requirements]()
 		- [macOS Build Requirements]()
 		- [Linux Build Requirements]()
 - [Building Sample Projects](#building-samples)
 	- [Building Samples with CMake (All Platforms)](#windows)
 	- [Building Samples (Windows)](#macOS)
 	- [Building Samples (macOS)](#linux)
 - [Using the C++ API](#using-the-c++-api)
 	- [Compiler / Linker Flags]()
 	- [API Headers]()
 	- [Platform Handlers]()
 		- [Config]()
 		- [GPUDriver]()
 		- [FontLoader]()
 		- [FileSystem]()
 	- [Update Loop]()
 	- [Managing Views]()
 		- [Loading Content]()
 		- [Input Events]()
 		- [Handling Events]()
 	- [JavaScript Interop]()
		- [JavaScriptCore API]()
		- [Obtaining a JSContext]()
		- [DOMReady Event]()
		- [Evaluating Scripts]()
		- [Calling JS Functions from C++]()
		- [Calling C++ Functions from JS]()
 - [Using the Framework Code]()
 	- [Select Framework Platform]()
	- [Writing Your Application]()
	- [JSHelpers Utility Code]()

# Getting Started

Before you get started, you will need the following on each platform:

## Build Requirements

All platforms include an __OpenGL-based sample (powered by GLFW)__. To build these cross-platform CMake/GLFW samples you will need:

 - CMake 2.8.12 or later
 - OpenGL 3.2 or later
 - Compiler with C++11 or later

### Windows Build Requirements

 - In addition to the above...
 	- Visual Studio 2015 or later
 	- DirectX 11+ or later __[optional], only for D3D11-based Samples__

### macOS Build Requirements
 - In addition to the above...
 	- XCode 8.0+ or later
 	- Metal 2 (macOS High Sierra or later) __[optional], only for Metal-based Samples__

### Linux Build Requirements

 - In addition to the above...
 	- 64-bit Debian-based OS (__Debian 9.5.0+__ OR __Ubuntu 12.04.5+__)


# Building Sample Projects

## Building Samples with CMake (All Platforms)

## Building Samples (Windows Only)

## Building Samples (macOS Only)

# Using the C++ API

## Compiler / Linker Flags

## API Headers

## Platform Handlers

### Config

### GPUDriver

### FontLoader

### FileSystem

## Update Loop

## Managing Views

### Loading Content

### Input Events

### Handling Events

## JavaScript Interop

### JavaScriptCore API

### Obtaining a JSContext

### DOMReady Event

### Evaluating Scripts

### Calling JS Functions from C++

### Calling C++ Functions from JS

# Using the Framework Code

## Select Framework Platform

## Writing Your Application

## JSHelpers Utility Code
