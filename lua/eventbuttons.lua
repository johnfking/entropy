--
-- entropoy.mac
-- events.lua
-- 
-- premade hotkeys
--


require "common"

local openGUI = true
local shouldDrawGUI = true

local butnum = {} 
butnum[3] = 'hide'
butnum[4] = 'aura'
butnum[5] = 'cure'
butnum[6] = 'home'
butnum[7] = 'say'


local imguicallback = function()
  openGUI, shouldDrawGUI = ImGui.Begin('Event', openGUI)
  if shouldDrawGUI then
    cmd_button('banner', 60, 22, 'event banner')
    ImGui.SameLine()
    cmd_button('campfire', 60, 22, 'event campfire')
    ImGui.SameLine()
    
    for i = 3,7 do
      cmd_button(butnum[i], 60, 22, 'event '..butnum[i])
      if (((i - 1) % 6) < 5) then ImGui.SameLine() end
    end
  end
  ImGui.End()
end

mq.imgui.init('Event', imguicallback)

while openGUI do 
  mq.delay(1000)
end