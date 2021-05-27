--
-- entropoy.mac
-- buttons.lua
-- 
-- premade hotkeys
--




require "common"


local openGUI = true
local shouldDrawGUI = true

local butnum = {} 
for i=1,16 do butnum[i] = i end

local imguitest = function()
    openGUI, shouldDrawGUI = ImGui.Begin('Test GUI', openGUI)
    if shouldDrawGUI then
        for i = 1,16 do
            ImGui.Button(butnum[i]..'###'..i, 40, 40)
            if ImGui.BeginDragDropSource() then
                ImGui.SetDragDropPayload('ITEMN', i)
                ImGui.Button('drag'..butnum[i], 40, 40)
                ImGui.EndDragDropSource()
            end
            if ImGui.BeginDragDropTarget() then
                local payload = ImGui.AcceptDragDropPayload('ITEMN')
                if payload ~= nil then
                    local num = payload.Data
                    local tmp = butnum[num]
                    table.remove(butnum, num)
                    table.insert(butnum, i, tmp)
                end
                ImGui.EndDragDropTarget()
            end
            if (((i - 1) % 4) < 3) then ImGui.SameLine() end
        end
        ImGui.End()
    end
end

mq.imgui.init('imguitest', imguitest)

while true do mq.delay(1) end