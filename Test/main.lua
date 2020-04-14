local ps = require 'calyx.processing'
local loadImage = ps.loadImage
local popMatrix = ps.popMatrix
local pushMatrix = ps.pushMatrix
local translate = ps.translate
local rotate = ps.rotate
local scale = ps.scale
local image = ps.image

--require 'module1'

TWO_PI = math.pi*2
mouseX = 10
mouseY = 70
stand1 = {}
attack1 = {}
run1 = {}

function loadImageRes(template, n)
    res = {}
    for i = 1, n do
        file = string.format('res/'..template, i)
        print('load resource '..file)
        res[i] = loadImage(file)
    end
    return res
end

function setup()
    printf("hello world\n")
    x = 0.1
    --ps.background(108, 108, 208)
    --ps.background(30, 82, 150)
    attack1 = loadImageRes('zs_001041_Attack_02_00%02d.png', 5)
    run1 = loadImageRes('sz_001006_Run_02_00%02d.png', 8)
    stand1 = loadImageRes('sz_001006_Prepare_02_00%02d.png', 4)
    box = loadImage('res/box.png')
end

count1 = 1
count2 = 1
count3 = 1
speed = 0
step = 9   -- 奔跑速度
posX = 0    -- 角色位置

function draw()
    -- 控制动画速度
    if speed > 9 then
        count1 = count1 + 1
        count2 = count2 + 1
        count3 = count3 + 1
        speed = 1
        posX = posX + step
        if posX > calyx.conf.app.width then
            posX = -100
        end
    end

    -- 重置动画帧指针
    if count1 > 5 then
        count1 = 1
    end

    if count2 > 8 then
        count2 = 1
    end

    if count3 > 4 then
        count3 = 1
    end

    -- 绘制动画
    ps.image(run1[count2], posX, 200)

    ps.image(attack1[count1], 300, 500)

    ps.pushMatrix()
    ps.scale(2)
    ps.image(stand1[count3], 300, 200)
    ps.popMatrix()

    ps.pushMatrix()

    ps.translate(100, 100)
    ps.popMatrix()

    --pushMatrix()
	-- 将图片平移
	--translate(500, 500)
    --imageMode(0)
	--scale(0.3)
	--rotate(x)
	--image(box, 100, 100)
	--popMatrix()


    ps.pushMatrix()
    fps = string.format("%0.2f", frameRate)
    ps.text('帧率: '..fps, 10, 100)
    ps.popMatrix()

    --angle = string.format("%0.2f", x/math.pi*180)
    --text('旋转角度: '..angle, 10, 10)
    ps.text("操作系统: "..calyx.os, 10, 10)
    ps.text("显卡信息: "..screen.videoCard, 10, 30)
    ps.text('屏幕宽度: '..screen.width, 10, 50)
    ps.text('屏幕高度: '..screen.height, 10, 70)

    --translate(100, 100)

    --pushMatrix()
    --rotate(0.3)
    --text("文字效果", 330, 150)
    --popMatrix()

    --ps.text(string.format("第 %d 帧", count2), posX, mouseY)
end

function update(dt)
    -- 更新帧率相关的变量
    speed = speed + 1
    x = (x + 0.02)%TWO_PI
end

function mousepressed(x, y, button)
    if button == "right" then
        mouseX = x
        mouseY = y
        --translate(x, y)
    end
    --print(button.." "..x.." "..y)
end

function mouserelease(x, y, button)
end

triggerPause = true

function keypressed(key, isrepeat)
    io.write(key)
    if key == " " then
        if triggerPause then
            calyx.event.post("pause")
            triggerPause = false
        else
            calyx.event.post("run")
            triggerPause = true
        end
    elseif key == 'c' then
        math.randomseed(os.time())
        r = math.random(0, 255)
        g = math.random(0, 255)
        b = math.random(0, 255)
        print(string.format('set background color %.0f %.0f %.0f', r, g, b))
        ps.background(r, g, b)
    elseif key == 'w' then
        io.write("加速")
        step = step + 10
    elseif key == 's' then
        io.write("减速")
        step = step - 10
    elseif key == 'q' then
        calyx.event.post("quit")
    end

    if isrepeat then
        --print('repeat keystroke')
    end
end
