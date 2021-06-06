# AGP-Engine

Rendering-oriented engine for Advanced Graphics Programming (AGP) subject of the Bachelor's Degree in Game Development at CITM, UPC.
This project consists in creating a deferred renderer for the second assignment of the subject.

Made by Carlos Peña and Sebastià Lopez.

## Deferred Shading
<p align="center">
<img src="https://github.com/ChronoByte/AGP-Engine/blob/main/Doc/image.PNG" >
</p>


## Effect Techniques

### Relief Mapping

<p align="center">
<img src="https://github.com/ChronoByte/AGP-Engine/blob/main/Doc/agp1.PNG" >
</p>
<p align="center">
<img src="https://github.com/ChronoByte/AGP-Engine/blob/main/Doc/agp3.PNG" >
</p>
<p align="center">
<img src="https://github.com/ChronoByte/AGP-Engine/blob/main/Doc/agp4.PNG" >
</p>
<p align="center">
<img src="https://github.com/ChronoByte/AGP-Engine/blob/main/Doc/agp5.PNG" >
</p>

### Multipass Bloom


**GitHub repository:** [https://github.com/ChronoByte/AGP-Engine](https://github.com/ChronoByte/AGP-Engine)


## Controls

### Camera navigation

   * **WASD:** Move camera Forward-Left-Backward-Right.
   * **QE:** Move camera Up-Down.
   * **Left Click (hold):** Rotate camera.
   * **Right Click (hold)** Orbit camera.

### Rendering configuration

  * Enable/Disable deferred shading
      * gPosition target
      * gNormals target
      * gAlbedo target
      * gDepth target
      * Bright Color texture
      * Blurred Bloom texture
  * Enable/Disable forward shading
  * Modify Relief Mapping parameters
      * Clip edges
      * Rotate quad
      * Heigh scale
      * Min layers /Max layers
      * Swap texture examples
  * Modify Bloom parameters
      * Enable/Disable bloom
      * Blur iterations
      * Threshold
  
## Shader files
* Relief Mapping & Bloom [shaders.glsl](https://github.com/ChronoByte/AGP-Engine/blob/main/Engine/WorkingDir/shaders.glsl)

## Members

* Carlos Peña Hernando [CarlosUPC](https://github.com/CarlosUPC)
* Sebastià Lopez Tenorio [Sebi-Lopez](https://github.com/Sebi-Lopez)
