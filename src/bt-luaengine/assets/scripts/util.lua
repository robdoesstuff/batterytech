-- Utility Functions

REFERENCE_WIDTH = 1280
REFERENCE_HEIGHT = 800

-- returns a string of all elements of a set
function setToString(set)
	if set == nil then
		return "nil"
	end
	local l = {}
	for e in pairs(set) do
		l[#l + 1] = e
	end
	return "{" .. table.concat(l, ", ") .. "}"
end

-- shallow copies a table
function table.copy(t)
	local u = {}
	for k, v in pairs(t) do u[k] = v end
	return u
end

-- returns true if str starts with start
function string.starts(str, start)
	return string.sub(str, 1, string.len(start)) == start
end

-- allocates unique metatable and index for an object which supports the function "replaceMetatable"
function allocMeta(o, newIndex)
	local mt = table.copy(getmetatable(o))
	local index
	if newIndex then
		index = table.copy(newIndex)
	else
		index = table.copy(mt.__index)
	end
	-- print("mt=" .. setToString(mt))
	-- print("index=" .. setToString(index))
	mt.__metatable = mt
	mt.__index = index
	mt.__newindex = function(t, key, value) 
		getmetatable(t).__index[key] = value 
	end
	o:replaceMetatable(mt)
	if o.properties then
		o.properties = table.copy(o.properties)
	else
		o.properties = {}
	end
	return o
end

function isValidGameObject(o)
	if not o then return false end
	return (getmetatable(o).__index and o.isActive)
end

function vec3_add(x1,y1,z1,x2,y2,z2)
	return x1+x2, y1+y2, z1+z2
end

function vec3_sub(x1,y1,z1,x2,y2,z2)
	return x1-x2, y1-y2, z1-z2
end

function vec3_lengthsquared(x,y,z)
	return x*x + y*y + z*z
end

function vec3_length(x,y,z)
	return math.sqrt(x*x + y*y + z*z)
end

function vec3_normalize(x,y,z)
	local length = math.sqrt(x*x + y*y + z*z)
	return x/length, y/length, z/length
end

function vec2_add(x1,y1,x2,y2)
	return x1+x2, y1+y2
end

function vec2_sub(x1,y1,x2,y2)
	return x1-x2, y1-y2
end

function vec2_lengthsquared(x,y)
	return x*x + y*y
end

function vec2_length(x,y)
	return math.sqrt(x*x + y*y)
end

function keys(t)
	local out = {}
	local i = 1
	for k,v in pairs(t) do
		out[i] = k
		i = i + 1
	end
	return out
end

function logtable(t)
	if type(t) ~= "table" then
		logmsg("*****"..tostring(t))
		return
	end
	for k,v in pairs(t) do
		if type(v) == "table" then
			logmsg("*****" .. tostring(k) .. ", ->")
			logtable(v)
		else
			logmsg("*****" .. tostring(k) .. ", " .. tostring(v))
		end
	end
end

function fields(t, ks)
	local function iter(i)
		if not(i > #ks) then
			return t[ks[i]], iter(i+1)
		else return end
	end
	return iter(1)
end

--type signature: Table (Either Bool (() -> Bool), () -> x) -> Maybe x
--	where x is something
--	since lambdas are so verbose in lua, i only use them here if i need something to be evaluated lazily, 
function switch(t)
	for k,v in ipairs(t) do
		if type(v[1]) == "function" then
			if v[1]() then return v[2]() end
		else
			if v[1] then return v[2]() end
		end
	end
end

function sign(x)
	if x >= 0 then return ("+"..tostring(x)) else return tostring(x) end
end

function lerp1(dst, src, t)
	return src + (t * (dst - src))
end

function lerp2(dstX, dstY, srcX, srcY, t)
	return lerp1(dstX, srcX, t), lerp1(dstY, srcY, t)
end

function polylineLerp(points, t)
	local ratios = {}
	local totalLength = 0
	for i,v in ipairs(points) do
		local u = points[i+1]
		if u then
			ratios[i] = ((u.x - v.x)^2 + (u.y - v.y)^2)^0.5
			totalLength = totalLength + ratios[i]
		end
	end
	for i,v in pairs(ratios) do
		ratios[i] = v/totalLength
	end
	local last = 1
	for i,v in ipairs(ratios) do
		last = i
		if v < t then t = t - v else break end
	end
	return lerp2(points[last+1].x, points[last+1].y, points[last].x, points[last].y, t)
end

--like table.copy but doesn't add keys
function setAttrs(t1, t2)
	for k,v in pairs(t1) do
		if t2[k] ~= nil then
			t1[k] = t2[k]
		end
	end
end

--sets the keybindings. takes a table of keycodes and functions, retuns a function that calls the associated function when a key is pressed (or released)
--Table KeyCodes (firstPressedFunction, firstReleastedFunction, isPressedFunction, isReleasedFunction)
function setKeyBindings(keyActions)
	local states = {}
	for k,v in pairs(keyActions) do
		states[k] = false
	end
	return function () 
		local statescpy = table.copy(states)
		--process key presses
		for i = 0, 10 do
			local isdown, code = getKeyState(i)
			if keyActions[code] then
				if isdown == true then
					statescpy[code] = nil
					if states[code] == false then
						if keyActions[code].firstPressedHandler then
							keyActions[code].firstPressedHandler()
						end
					elseif keyActions[code].isPressedHandler then
						keyActions[code].isPressedHandler()
					end
					states[code] = true
				end
			end
		end --for i = 0, 10 do
		--process key releases
		for k,v in pairs(statescpy) do
			if states[k] == true then --key was just released
				if keyActions[k].firstReleasedHandler then
					keyActions[k].firstReleasedHandler()
				end
			else
				if keyActions[k].isReleasedHandler then
					keyActions[k].isReleasedHandler()
				end
			end
			states[k] = false
		end
	end
end

function orderedKeys(t, f)
	local out = {}
	local i = 1
	for k,v in pairs(t) do
		out[i] = {}
		out[i][k] = v
		i = i + 1
	end
	table.sort(out, f)
	for k,v in pairs(out) do
		local key, _ = next(v)
		out[k] = key
	end
	return out
end

function table.toString(t, name)
	function f(t, name)
		typet = type(t)
		if typet == "nil" or typet == "number" or typet == "boolean" then
			return name.."="..tostring(t)..","
		elseif typet == "string" then
			return name.."=\""..tostring(t).."\","
		elseif typet == "table" then
			name = name.."={"
			if not next(t) then
				name = name.."},"
				return name
			end
			for k,v in pairs(t) do
				key = ""
				if type(k) == "string" then
					key = "['"..k.."']"
				else
					key = "["..tostring(k).."]"
				end
				name = name..f(v, key)
			end
			name = name:sub(1, #name-1)
			name = name.."},"
			return name
		end
	end
	
	local out = f(t, name)
	return out:sub(1, #out-1)
end

function max(list)
	local m = nil
	for k,v in pairs(list) do
		if m then
			if v > m then
				m = v
			end
		else
			m = v
		end
	end
	return m
end

--taken from lua users wiki
function deepcopy(object)
	local lookup_table = {}
	local function _copy(object)
		if type(object) ~= "table" then
			return object
		elseif lookup_table[object] then
			return lookup_table[object]
		end
		local new_table = {}
		lookup_table[object] = new_table
		for index, value in pairs(object) do
			new_table[_copy(index)] = _copy(value)
		end
		return setmetatable(new_table, getmetatable(object))
	end
	return _copy(object)
end

-- Resizes a texture to fit inside maxWidth, maxHeight preserving its aspect ratio
function resizePreserveAspectRatio(texWidth, texHeight, maxWidth, maxHeight)
	if texWidth > maxWidth then texWidth, texHeight = maxWidth, maxHeight * (texHeight/texWidth) end
	if texHeight > maxHeight then texWidth, texHeight = maxWidth * (texWidth/texHeight), maxHeight end
	return texWidth, texHeight
end

function scaleXY(x, y)
	local vpWidth, vpHeight = getViewportSize()
	return x * (vpWidth/REFERENCE_WIDTH), y * (vpHeight/REFERENCE_HEIGHT)
end

function scaleX(x)
	local vpWidth, vpHeight = getViewportSize()
	return x * (vpWidth/REFERENCE_WIDTH)
end

function scaleY(y)
	local vpWidth, vpHeight = getViewportSize()
	return y * (vpHeight/REFERENCE_HEIGHT)
end

function scaleForUI(x, y)
	local vpWidth, vpHeight = getViewportSize()
	local scaleFactor = vpHeight/REFERENCE_HEIGHT
	return x * scaleFactor, y * scaleFactor
end

function getUVs(idx, rows, columns)
	local row = math.floor(idx / rows)
	local column = idx % columns
	return column / columns, row / rows, (column+1) / columns, (row+1) / rows
end

function sigmoid(x)
    -- needs a little help, this almost works but not perfectly
    local expansion = 1.1
    x = ((x - .5) * expansion) + .5
    if x < 0 then x = 0 elseif x > 1 then x = 1 end
    return (x / math.sqrt(1 + x*x))
end
