# Gato
- [About](#about)
	- [Features](#features)
	- [Stuff I want to do](#to-do)
- [Documentation](#documentation)
	- [Structure](#structure)
	- [Globals](#globals)
	- [Classes](#classes)

# About
Gato is a minimalistic 3D game engine with Lua scripting. Gato was made so I could learn about graphics and desktop app development. The engine is not very mature yet but I have hopes for it in the future.

## Features
- 3D OpenGL Renderer
- Dear ImGui Interface including:
	- Object tree view
	- Resource folders
	- Inspector
	- Lua Console
- Save system stores engine state across sessions 
- .obj parser
- glsl shaders
- LuaJIT scripting

## To Do
- Physics
- Multithreading
- Compute Shaders(I'll have to wait until I own a computer that supports them)
- Prettier GUI
- a more sofisticated library
- some sort of debugging
- settings
- make it targetable by Emscripten
- make sure it works on all types of OS

# Documentation
## Structure
Gato gives you access to a console in which you can run Lua code on demand but the primary way to add functionality to your game is by making scripts.

All scripts will be of the form:
in MyScript.lua
```lua
-- This is your Script. Take care of it
MyScript={}

-- This is called when the game starts
MyScript.onStart=function()
	--set up for the cool stuff
end

-- This is called once per frame
MyScript.onUpdate=function()
	--do cool stuff
end
```
Each script should have its own responsibility. For example, in a first person shooter there should be individual scripts for handling movement, AI, bullet physics, etc.

## Globals
There are 6 tables Gato makes available to the user:
- objects
- lights
- meshes
- shaders
- textures
- audioBuffers

and 2 objects:
- mainRenderer
- screen

The tables map strings to their corresponding class.
These tables are used to register objects into the game engine. Objects will not be drawn unless in these tables or unless you draw them yourself.  Functions to emplace into these tables are provided:
- shaders_emplace(name, args...)
- textures_emplace(name, args...)
- meshes_emplace(name, args...)
- objects_emplace(name, args...)
- lights_emplace(name, args...)
- audioBuffers_emplace(name, args...)

It is recommended to use these as arguments are forwarded to the corresponding constructor without making any temporary variables. They output a table: {successful,obj} where successful is true and obj is the new object if the object was inserted, successful is false and obj is the existing object if there was already an object with the same name. 

```lua
--example
local newObject=objects_emplace("Bob",meshes["bobsMesh"], shaders["bobsShader"], textures["bobsTextures"], vec3.new(0))
if newObject.successful then
	--successfully inserted
else 
	--object with the name Bob already exists
end
```
screen is an Object with a rectangular mesh and the shader "screen.shader" attached to it. Modify "screen.shader" to do post processing. Change the screen's Transform for shaking effects, etc. 
```lua
--example
screen:setShader(shaders["mySpookyShader"])
screen.transform:translate(vec3.new(dx,dy,dz))
```
mainRenderer is the renderer which draws the scene in a FrameBuffer which has screen's texture associated with it. You can draw directly to the mainRenderer if you'd like.
```lua
--example
myObject=Object.new(PreMadeMesh.Sphere,vec3.new(0))
mainRenderer:draw(myObject)
```

## Classes
- [Mesh](#mesh)
	- [PreMadeMesh](#enum-premademesh)
- [Shader](#shader)
- [Texture](#texture)
- [Camera](#camera)
- [Controller](#controller)
	- [ControllerType](#enum-controllertype)
- [Frame Buffer](#framebuffer)
- [Renderer](#renderer)
- [Transform](#transform)
- [Object](#object)
- [Light](#light)
	- [LightType](#enum-lighttype)
- [AudioBuffer](#audiobuffer)
- [AudioSource](#audiosource)
- [Math](#math)
	- [vec4](#vec4)
	- [vec3](#vec3)
	- [vec2](#vec2)
	- [mat4](#mat4)
	- [mat3](#mat3)
	
##### enum PreMadeMesh
		Capsule, Cone, Cube, Cylinder, Disk, Icosahedron, Quartz, Sphere, Square
#### Mesh
		new(string filePath,int dimensions,bool hasTexture,bool hasNormals)
		new(arr[float] vertices,arr[uint] indices,int dimensions,bool hasTexture,bool hasNormals)
		new(PreMadeMesh)
		vertices->arr[float]
		indices->arr[uint]
```lua
--example
myMesh=Mesh.new("myMesh.obj",3,true,true)
print(myMesh.vertices)
```
		
#### Shader
		new(string filePath)->Shader
		new()->Shader
		setInteger(string name,int x)
		setFloat(string name,float x)
		setFloat(string name,float x,float y,float z)
		setLight(Light x)
		setMatrix(string name,mat4 x)
		setTexture(string name,Texture x)
		createShaders(int numberOfShaders,string filePath)->arr[Shader]
		
```lua
--example
myShader=Shader.new("path/to/shader.shader")
myShader:setInteger("distance", 10)
shaders["myShader"]=myShader
```
		
#### Texture
		new()
		new(filePath)->Texture
		new(arr[string] filePaths)->Texture
		getWidth()->int
		getHeight()->int
		
```lua
--example
local filePaths={"path/to/Left.png", "path/to/Right.jpg", "path/to/Down.gif", "path/to/Up.png", "Front", "path/to/Back.jpeg"}
myCubeTexture=Texture.new(filePaths)
objects["skybox"].setTexture(myCubeTexture)
```
#### Camera
		new(vec3 position)->Camera
		transform->Transform
		view()->mat4
		controller->Controller
```lua
--example
dx=.05
MyScript.onUpdate=function()
	mainRenderer.camera.transform:translate(vec3.new(dx,0,0))
	local x=mainRenderer.camera.transform.position.x
	if math.abs(x)>5 then
		dx=-dx
	end
end
```
##### enum ControllerType
		None,FirstPersonFlying
#### Controller
		new(ControllerType)->Controller
		setToNone()
		setToFirstPersonFlying()
		sensitivity->float
		speed->float
```lua
--example
mainRenderer.camera.controller:setToNone()
```
		
#### FrameBuffer
		new(Texture color,Texture depth,int width,int height,bool isCubeTexture)
		new(Texture color,int width,int height,bool isCubeTexture)
		new(bool placeHolder,Texture depth,int width,int height,bool isCubeTexture)
			
#### Renderer
		new(FrameBuffer buffer, float fov)->Renderer
		new(int width,int height,float fov)->Renderer
		clear()
		clear(vec3 color)
		draw(Object obj)
		draw(arr[Object] objList)
		draw(table{string:Object} objMap)
		draw2D(Object obj)
		draw2D(arr[Object] objList)
		draw2D(table{string:Object} objMap)
		camera->Camera
		
```lua
--example
MyScript={}

MyScript.onStart=function()
	someObjects={}
	for i=0,10 do
		someObjects[i]=Object.new(PreMadeMesh.Cube,vec3.new(i*.2,math.random(),10))
	end
	
	color=Texture.new()
	depth=Texture.new()
	myFrame=FrameBuffer.new(color,depth,920,640,false)
	myRenderer=Renderer.new(myFrame,90)
	objects_emplace("shadowWindow",PreMadeMesh.Square,shaders["defaultShader"],depth,vec3.new(1))
end

MyScript.onUpdate=function()
	myRenderer.clear()
	myRenderer.draw(someObjects)
	myRenderer.camera.transform:rotate(.05,vec3.new(1))
end
```
#### Transform
		new()->Transform
		new(vec3 position,vec3 orientation)->Transform
		translate(vec3 direction)
		scale(vec3 factor)
		setPosition(vec3 newPosition)
		rotate(float angle,vec3 axis)
		setOrientation(vec3 newOrientation)
		setScale(vec3 newScale)
		addChild(string name,Transform child)
		removeChild(string name)
		removeChild(Transform child)
		view()->mat4
		position->vec3
		orientation->vec3
		shape->vec3
		childrenNames->arr[string]
```lua
--example
bob=objects["Bob"]
bobsSon=objects["Jimmy"]
bob.transform:addChild("Jimmy",bobsSon.transform)
bob.transform:setScale(100)--Jimmy's scale is now also 100
```
		
#### Object
		new(Mesh m,Shader s,Texture t,vec3 position)->Object
		new(Mesh m,Shader s,vec3 position)->Object
		new(Mesh m,vec3 position)->Object
		new(PreMadeMesh m,Shader s,Texture t,vec3 position)->Object
		new(PreMadeMesh m,Shader s,vec3 position)->Object
		new(PreMadeMesh m,vec3 position)->Object
		setMesh(Mesh newMesh)
		setShader(Shader newShader)
		setTexture(Texture newTexture)
		setShadow(bool hasShadow)
		hasShadow()->bool
		transform->Transform
		audio->AudioSource
		
##### enum LightType
		Point, Direct
#### Light	
		new(vec3 position,vec3 orientation,vec3 color,float intensity,LightType type,Shader depthShader)->Light
		setColor(vec3 newColor)
		setDepthShader(Shader newShader)
		setIntensity(int newIntensity)
		clear()
		transform->Transform
		draw(Object obj)
		draw(arr[Object] objList)
		draw(table{string:Object} objMap)
		
```lua
--example
MyNewScript={}

MyNewScript.onStart=function()
	sun = lights_emplace("sun",vec3.new(0),vec3.new(1),vec3.new(1,0,1),1,LightType.Point,shaders["depthShader"]).obj
    cubeTry = objects_emplace("light cube",PreMadeMesh.Cube,vec3.new(0))
    if cubeTry.successful then
        lc=cubeTry.obj
        lc:setShader(shaders["defaultShader"])
        sun.transform:addChild("light cube",lc.transform)
        lc.hasShadow = false
        lc.transform:setScale(vec3.new(.1))
    end
end

MyNewScript.onUpdate=function()
	sun=lights["sun"]
	--this must be done for all lights to use built in lightShader
	sun:clear()
    sun:draw(objects)

    shaders["lightShader"]:bind()
    shaders["lightShader"]:setLight(sun)
end
```
#### AudioBuffer
		new(string filePath)->AudioBuffer
		setSound(string filePath)

#### AudioSource
		playSound()
		setBuffer(AudioBuffer buff) 
		removeBuffer()
		setPitch(float newPitch)
		setGain(float newGain)
		setPosition(vec3 pos)
		setVelocity(vec3 vel)
		setLooping(bool isLooping)
```lua
--example
audio=AudioBuffer.new("path/to/sound.wav")
emptyObject=Object.new(vec3.new(0))
emptyObject.audio:setBuffer(audio)
emptyObject.audio:playSound()
```

##### vec3
		new(float x)->vec3
		new(float x,float y,float z)->vec3
##### vec2
		new(float x)->vec2
		new(float x,float y)->vec2
##### vec4
		new(float x)->vec4
		new(float x,float y,float z,float w)->vec4
##### mat3
		new(vec3 x,vec3 y,vec3 z)->mat3
##### mat4
		new(vec4 x,vec4 y,vec4 z,vec4 w)->mat4

#### Math
		mult(mat4 x,mat4 y)->mat4
		mult(mat4 x,vec4 y)->vec4
		mult(vec4 x,mat4 y)->vec4
		mult(vec4 x,vec4 y)->vec4
		mult(float x,vec4 y)->vec4
		mult(mat3 x,mat3 y)->mat3
		mult(mat3 x,vec3 y)->vec3
		mult(vec3 x,mat3 y)->vec3
		mult(vec3 x,vec3 y)->vec3
		mult(float x,vec3 y)->vec3
		mult(mat2 x,mat2 y)->mat2
		mult(mat2 x,vec2 y)->vec2
		mult(vec2 x,mat2 y)->vec2
		mult(vec2 x,vec2 y)->vec2
		mult(float x,vec2 y)->vec2
		
		add(mat4 x,mat4 y)->mat4
		add(vec4 x,vec4 y)->vec4
		add(float x,vec4 y)->vec4
		add(vec4 x,float y)->vec4
		add(mat3 x,mat3 y)->mat3
		add(vec3 x,vec3 y)->vec3
		add(float x,vec3 y)->vec3
		add(vec3 x,float y)->vec3
		add(mat2 x,mat2 y)->mat2
		add(vec2 x,vec2 y)->vec2
		add(float x,vec2 y)->vec2
		add(vec2 x,float y)->vec2
		
		subtract(mat4 x,mat4 y)->mat4
		subtract(vec4 x,vec4 y)->vec4
		subtract(float x,vec4 y)->vec4
		subtract(vec4 x,float y)->vec4
		subtract(mat3 x,mat3 y)->mat3
		subtract(vec3 x,vec3 y)->vec3
		subtract(float x,vec3 y)->vec3
		subtract(vec3 x,float y)->vec3
		subtract(mat2 x,mat2 y)->mat2
		subtract(vec2 x,vec2 y)->vec2
		subtract(float x,vec2 y)->vec2
		subtract(vec2 x,float y)->vec2
		
		transpose(mat4 x)->mat4
		transpose(mat3 x)->mat3
		
		inverse(mat4 x)->mat4
		inverse(mat3 x)->mat3
		
		dot(vec4 x,vec4 y)->float
		dot(vec3 x,vec3 y)->float
		dot(vec2 x,vec2 y)->float

		length(vec3 x)->float
		length(vec3 x)->float
		length(vec3 x)->float

		cross(vec3 x,vec3 y)->vec3
```lua
--example
local x=vec3.new(1,2,3)
local A=mat3.new(vec3.new(2),vec3.new(3),vec3.new(4))
print(math.mult(A,x))
```
