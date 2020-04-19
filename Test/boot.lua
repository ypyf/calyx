-- 创建内建名字空间
calyx.event	= {}

-- 缺省配置
calyx.conf = {
	app = {
		title = "Calyx 3D",
		version = calyx.version,
		width = 800,
		height = 600,
		minwidth = 1,
		minheight = 1,
		fullscreen = false,
		fullscreentype = "normal",
		display = 1,
		vsync = true,
		fsaa = 0,
		borderless = false,
		resizable = false,
		centered = true,
		highdpi = false,
		srgb = false,
		console = false, -- Only relevant for windows.
		identity = false,
		icon = nil
	}
}

-- 载入程序配置
function Application(config)
	for k, v in pairs(config) do
		calyx.conf.app[k] = v
	end
end

function calyx.init()
	-- 加入当前路径
	package.path = package.path .. ";?.lua"
	-- 载入配置文件
	dofile("config.lua")
end

local function deferErrhand(...)
	local handler = love.errhand or error_printer
	return handler(...)
end

-- 返回后要调用这个函数
-- TODO 错误处理
return pcall(calyx.init)
