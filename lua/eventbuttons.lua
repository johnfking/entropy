--
-- entropoy.mac
-- eventbuttons.lua
-- 
-- premade hotkeys for raid designed events.
--


require "common"

local openGUI = true
local shouldDrawGUI = true

local imguicallback = function()
  openGUI, shouldDrawGUI = ImGui.Begin('event##eventbuttons', openGUI, ImGuiWindowFlags.NoTitleBar + ImGuiWindowFlags.NoScrollbar)
  if shouldDrawGUI then
    cmd_button(ico.banner, ico.x, ico.y, 'event banner', 'move to banner')
    ImGui.SameLine()
    cmd_button(ico.campfire, ico.x, ico.y, 'event campfire', 'move to campfire')
    ImGui.SameLine()
    
    cmd_button(ico.hide, ico.x, ico.y, 'event hide', 'hide from emote')
    ImGui.SameLine()
    cmd_button(ico.aura, ico.x, ico.y, 'event aura', 'move to aura')
    ImGui.SameLine()
    cmd_button(ico.cure, ico.x, ico.y, 'event cure', 'cure self')
    ImGui.SameLine()
    cmd_button(ico.home, ico.x, ico.y, 'event home', 'move to home marker')
    ImGui.SameLine()
    cmd_button(ico.say, ico.x, ico.y, 'event say', 'move to and speak')

  end
  ImGui.End()
end

mq.imgui.init('Event', imguicallback)

while openGUI do 
  mq.delay(1000)
end