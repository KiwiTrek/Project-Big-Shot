# Project-Big-Shot
Engine created for the "Game Engines" subject in the Bachelor's degree in Video Game Design and Development at the CITM-UPC center, Barcelona

### Link: https://github.com/WillyTrek19/Project-Big-Shot

# Team Members:
 - Guillem Álava Burgueño ([@WillyTrek19](https://github.com/WillyTrek19))
 - Sergi Colomer Ferrer ([@Lladruc37](https://github.com/Lladruc37))
 - **New!** Arnau Lucena Cobo ([@TitoLuce](https://github.com/TitoLuce))

# Keyboard Controls:
### General
 - ESC: Exit engine
 - **New!** Left click:
   - Drag and drop elements on the scene to import them
   - Mouse picking & gizmo manipulation
   - Drag a resources from the assets panel to a component to replace it
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
 - **New!** F2: Show asstes panel
 - **New!** F3: Show scene panel
   - **New!** W: Translate
   - **New!** E: Rotate
   - **New!** R: Scale
 - F4: Show hierarchy panel
    - Return: Select game object
    - Delete: Erase selected game object in hierarchy
    - **New!** Shift + Up/Down: Move Up/Down
 - F5: Show inspector panel
 - F10: Show console

# Panel controls:
Each of the panels has a set of available actions.
## Menu Bar
### File
 - **New!** Load Scene
 - **New!** Save Scene
 - Quit application
### View
 - Show panels (Showed below)
### Create
 - Create empty game objects
 - Create primitives
 - **New!** Create camera
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
### _New!_ Editor Camera
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
   - **New!** Show grid
     - Set transparency
   - Color Material
   - Texture 2D
    - **New!** Draw Mouse raycast
## Hierarchy
  - **New!** Drag & Drop to reparent
  - **New!** When right clicking, a pop-up will appear that shows the following options
    - Move object up & down the hierarchy
    - Delete the object
    - Create a gameobject that is child to the selected object
## Inspector
### When selecting an object:
  - Change its name
### Components
#### Transform
  - **New!** Set gizmo operation
  - Set its position, rotation and scale*
  - Set its scale uniformly*
  - Reset to 0

*Scale is disabled when manipulating camera

#### _New!_ Camera
  - Set as main camera
  - Toggle frustum culling
  - Set fixed FOV
  - Change FOV based on fixed FOV
  - Set near & far planes
 
#### Material
  - Toggle it
  - Set the checkers texture
  - **New!** Drag and drop asset to replace
  - **New!** Remove it
#### Mesh
  - Toggle it
  - **New!** Enable wireframe mode only on that model
  - Draw vertex & face normals
  - **New!** Drag and drop asset to replace
  - **New!** Remove it
## About
  - Click on hyperlinks

# Additional Features & Notes
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
