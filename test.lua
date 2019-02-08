
-- test.lua
-- lua starts at 1 not 0
local m = { }



wks_configurations = {}


m.WORKSPACE = 0
m.PROJECT = 1


-- takes a string
function m.setWorkspace( workspaceName ) 
    workspace( workspaceName )
end

function m.setSolutionName( solutionName )
    filename( solutionName )
end

function m.addConfig( configuration )
    -- table.insert(wks_configurations, configuration)
    -- print(wks_configurations[1]) --lua starts at 1 not 0
    -- print(wks_configurations[2])
    table.insert(configurations, configuration)
end

function m.setPlatform( platform )
    platforms { platform }
end


return m