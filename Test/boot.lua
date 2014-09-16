-- �����ڽ����ֿռ�
calyx.event	= {}

-- ȱʡ����
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

-- �����������
function Application(config)
	for k, v in pairs(config) do
		calyx.conf.app[k] = v
	end
end

function calyx.init()
	-- ���뵱ǰ·��
	package.path = package.path .. ";?.lua"
	-- ���������ļ�
	dofile("config.lua")
end

local function deferErrhand(...)
	local handler = love.errhand or error_printer
	return handler(...)
end

-- ���غ�Ҫ�����������
-- TODO ������ 
return pcall(calyx.init)