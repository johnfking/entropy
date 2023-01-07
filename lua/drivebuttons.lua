--
-- entropoy.mac
-- drive.lua
-- 
-- driving toon
--


require 'common'
local openGUI = true
local shouldDrawHUD = true


local function imguicallback()
  hudInfo()
  openGUI, shouldDrawHUD = ImGui.Begin('Drive Controls###EntropyDriveButtons', openGUI, ImGuiWindowFlags.NoTitleBar + ImGuiWindowFlags.NoScrollbar)
  
  if shouldDrawHUD and (ent.build == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then

  elseif shouldDrawHUD then  
 
    ImGui.Columns(6, 'buttons', false)
      cmd_button(ico.allauto, ico.x, ico.y, 'on', 'all macro auto')
      ImGui.SameLine()
      cmd_button(ico.allmanual, ico.x, ico.y, 'off', 'all macro manual')
      ImGui.SameLine()
      cmd_button(ico.mqp, ico.x, ico.y, 'dga /mqp', 'all macro pause')

    -- 2
    ImGui.NextColumn()
      cmd_button(ico.tie, ico.x, ico.y, 'dga /tie', '/tie on/off')
      ImGui.SameLine()
      cmd_button(ico.tienav, ico.x, ico.y, 'dga /tie nav', 'tie with nav')
      ImGui.SameLine()
      cmd_button(ico.tiestick, ico.x, ico.y, 'dga /tie stick', 'tie with stick')

    --3
    ImGui.NextColumn()
     if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
        cmd_button(ico.manual, ico.x, ico.y, 'env auto', 'macro manual')
      else 
        cmd_button(ico.auto, ico.x, ico.y, 'env auto', 'macro auto')
      end
      ImGui.SameLine()
      cmd_button(ico.incharge, ico.x, ico.y, 'env incharge', '/incharge')
      ImGui.SameLine()
      cmd_button(ico.campfire, ico.x, ico.y, 'home campfire incharge', 'drop campfire')

    -- 4
    ImGui.NextColumn()
      cmd_button(ico.invis, ico.x, ico.y, 'invis', 'stack invisibility')
      ImGui.SameLine()
      cmd_button(ico.noinvis, ico.x, ico.y, 'dga /nomore invis', 'remove all invisibility')
      ImGui.SameLine()
      cmd_button(ico.nolevi, ico.x, ico.y, 'dga /nomore levi', 'remove all levitation')
    
    -- 5
    ImGui.NextColumn()
      --local myname = mq.TLO.Me.DisplayName() or ''
      cmd_button(ico.assist, ico.x, ico.y, 'dge /assist ' .. (mq.TLO.Me.DisplayName() or ' '), 'all /assist')
      ImGui.SameLine()
      cmd_button(ico.hail, ico.x, ico.y, 'dga /hail', 'all /hail')
      ImGui.SameLine()
      cmd_button(ico.intpull, ico.x, ico.y, 'pull one int', 'single intervention pull')
 
    -- 6
    ImGui.NextColumn()
      cmd_button(ico.autoinventory, ico.x, ico.y, 'dga /autoinventory', 'everyone autoinventoy')
      ImGui.SameLine()
      cmd_button(ico.gather, ico.x, ico.y, 'gather', '/gather')
      ImGui.SameLine()
      cmd_button(ico.here, ico.x, ico.y, 'here', '/here')
 
    ImGui.Columns()
  
  end
  
  ImGui.End()
  
end



mq.imgui.init('drivewindowcontrols', imguicallback)

while openGUI do 
  mq.delay(1000)
end