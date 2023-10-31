
-- This is your Script. Take care of it
MyNewScript={}
-- you have access to all game objects
-- you can even make some here, 
-- just make sure to register them

-- This is called when the game starts
MyNewScript.onStart=function()
	sun=lights["sun"]
    try = objects_emplace("light cube",PreMadeMesh.Cube,vec3.new(0))
    if try.successful then
        lc=try.obj
        print('hi')
        lc:setShader(shaders["defaultShader"])
        sun.transform:addChild("light cube",lc.transform)
        lc.hasShadow = false
        lc.transform:setScale(vec3.new(.1))
        
    end
end

-- This is called once per frame
MyNewScript.onUpdate=function()
	sun=lights["sun"]
    --steve=lights["steve"]
	sun:clear()
    --steve:clear()
    sun:draw(objects)
    --steve:draw(objects)

    shaders["lightShader"]:bind()
    --shaders["lightShader"]:setLight(steve)
    shaders["lightShader"]:setLight(sun)
end
