# Project-Big-Shot
Engine created for the "Game Engines" subject in the Bachelor's degree in Video Game Design and Development at the CITM-UPC center, Barcelona

### Link: https://github.com/WillyTrek19/Project-Big-Shot

# Team Members:
 - Guillem Álava Burgueño ([@WillyTrek19](https://github.com/WillyTrek19))
 - Sergi Colomer Ferrer ([@Lladruc37](https://github.com/Lladruc37))
 - Arnau Lucena Cobo ([@TitoLuce](https://github.com/TitoLuce))

# Keyboard Controls:
### General
 - ESC: Exit engine
 - Left click:
   - Drag and drop elements on the scene to import them
   - Mouse picking & gizmo manipulation
   - Drag a resources from the assets panel to a component to replace it
 - **New!** 1: Ignite Firework
### Camera
 - Right Click: Rotate Camera
 - Right Click +:
   - W: Move camera forward
   - A: Move camera left
   - S: Move camera right
   - D: Move camera back
   - Q: Move camera upwards
   - E: Move camera downwards
   - Shift: Duplicate camera speed
 - Mouse Wheel: Zoom camera (Same as W/S)
 - F: Look at camera
 - Alt + Left Click: Rotate around selected object
### Panels
 - Ctrl + Left Click: Edit values of sliders
 - Right click: Extra functionalities
 - F1: Show configuration panel
 - F2: Show asstes panel
 - F3: Show scene panel
   - W: Translate
   - E: Rotate
   - R: Scale
 - F4: Show hierarchy panel
    - Return: Select game object
    - Delete: Erase selected game object in hierarchy
    - Shift + Up/Down: Move Up/Down
 - F5: Show inspector panel
 - F10: Show console

# Panel controls:
Each of the panels has a set of available actions.
## Menu Bar
### File
 - Load Scene
 - Save Scene
 - Quit application
### View
 - Show panels (Showed below)
### Create
 - Create empty game objects
 - Create primitives
 - Create camera
 - **New!** Create particle emitter
### Help
 - Show GUI Demo Window
 - Go to source code
 - Go to donwload latest release
 - Go to issues window to report a bug
 - Show the about page

## Configuration
### Application
 - Set project name & organization name
 - Set maximum fps
### Window
 - Set brightness
 - Set window size (Width & Height)
 - Set fullscreen
 - Set the window to be resizable
 - Set borderless window
### Editor Camera
 - Set speed
 - Set sensitivity
 - Set Vertical FOV
 - Set Near & Far planes
### Render
 - Toggle:
   - VSync
   - Wireframe on all meshes
   - Depth Test
   - Cull Face
   - Lighting
   - Show grid
     - Set transparency
   - Color Material
   - Texture 2D
   - Draw Mouse raycast
## Hierarchy
  - Drag & Drop to reparent
  - When right clicking, a pop-up will appear that shows the following options
    - Move object up & down the hierarchy
    - Delete the object
    - Create a gameobject that is child to the selected object
## Inspector
### When selecting an object:
  - Change its name
### Components
#### Transform
  - Set gizmo operation
  - Set its position, rotation and scale*
  - Set its scale uniformly*
  - Reset to 0

*Scale is disabled when manipulating camera

#### Camera
  - Set as main camera
  - Toggle frustum culling
  - Set fixed FOV
  - Change FOV based on fixed FOV
  - Set near & far planes
 
#### Material
  - Toggle it
  - Set the checkers texture
  - Drag and drop asset to replace
  - Remove it
#### Mesh
  - Toggle it
  - Enable wireframe mode only on that model
  - Draw vertex & face normals
  - Drag and drop asset to replace
  - Remove it
### **New!** Particle Emitter
  - Edit particle's values (such as speed, acceleration, rotation, life, size...)
  - Change the emitter's shape/zone of movement
  - Change the particle's beginning and ending colors
  - Set whether or not particles appear in bursts
    - Set the range of particles that can appear
    - Set the rate of bursts
  - Set the particle material
    - Functionality mostly the same as material component
  - Set a sub emitter, which will create more particles on particle death
    - Set the sub emitter's values

## About
  - Click on hyperlinks

# Additional Features & Notes
## Assignment 0.3
 - The firework input initiates the firework emitter; but it needs one second to actually throw a firework. In fact, more fireworks could be thrown if the duration of the emitter is increased.
 - The duration value cannot be properly tested, as no "game mode" is implemented into the engine. However, we believe it should work as intended, as the emitter stops whenever the loop option is deactivated.
 - Save and Load functionalities don't support the firework emitters, so when trying to actually save and load data, the fireworks wont be erased nor created. The user will lose control of them, being unable to change the state in which they're currently in.
 - The bbox shown is not the actual particle's zone of movement, but rather a placeholder to click on the particle emitter and move it around.
 - Particles will point into the direction of the main camera, and rotate so that they're always looking at it. They're also affected by camera culling. However, the spawn position is not the center, but rather on the bottom-right of the particle plane (like the plane primitive), so, depending on the rotation of the camera, the particles may be displaced.

## Assignment 0.2
 - Some functionalities have been improved since the last assignment. For example, camera movements have been optimized & several controls saved inside menus are now inside right click popups or drag & drops.
 - Part of the scene doesn't load due to having more than 4 vertices.
 - When saving scene, an error may occur if the models included in the file have a different UID than those imported (in which they won't be recognized). If you want to solve this issue, you have to save the scene again including the models with the new UIDs.
 - Resources created with our file format (i.e. meshes) are saved inside the same "Assets" folder. Scenes are saved inside "Assets/Scenes".
 - Even though Materials are not saved using our own format, code is provided to showcase our progress towards the matter (with comments).
 - Engine only allows to save and load one scene at a time. If you want to save more than one scene, you'll have to rename it. The filename currently supported is **"currentScene.bss"**.
 - When disabling scene, visual bugs may ensue. They get solved when enabling the scene again.

## Assignment 0.1
 - Camera also allows for vertical movement (With Q/E) and resetting by pressing Shift + R.
 - Most panels have additional information detailing specs of the engine and the hardware.
 - The hierarchy has more functionalities than those required for the final delivery (moving objects, deleting them and creating empty game objects)
 - The inspector is not only for display purposes. For instance, the transform compononent allows for the translation, rotation and scaling of the object, and the other components have other controls as well.
 - The configuration window has similar functionalities as the ones of the examples shown in class when.
 - Some of the primitives' textures do not work properly, as their texture coordinates are not properly set. Others just don't have any. The sphere primitive is done in Direct Mode.
