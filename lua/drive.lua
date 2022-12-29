--
-- entropoy.mac
-- drive.lua
-- 
-- driving toon
--


require 'common'
local openGUI = true
local shouldDrawHUD = true



info = {}
info['channel'] = mq.TLO.Macro.Variable('maEntropy').Find('stEntropyGroup_all').Value()
info['build'] = mq.TLO.Macro.Variable('maEnv').Find('build').Value() or '--'

function driveinfo ()
  for i = 1, mq.TLO.DanNet.PeerCount(info['channel'])() 
  do

    toon = mq.TLO.DanNet.Peers(info['channel']).Arg(i,'|')()
    
   
    -- distance
    if mq.TLO.Spawn(toon).Distance() == nil then
      info[toon .. 'distance'] = 0
    else
      info[toon .. 'distance'] = round(mq.TLO.Spawn(toon).Distance(), 0)
    end

    -- state
    if mq.TLO.Spawn(toon).Dead() then
      info[toon .. 'state'] = 0  
      goto continue
    elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'TRUE' then
      info[toon .. 'state'] = 1
    elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'FALSE' then
      info[toon .. 'state'] = 2
    else
      info[toon .. 'state'] = 3
    end

    -- casting
    if mq.TLO.DanNet(toon).Observe('Me.Casting.ID')() ~= 'NULL' then
      info[toon .. 'casting'] = mq.TLO.DanNet(toon).Observe('Me.Casting.ID')()
    else
      info[toon .. 'casting'] = ''
    end
      


    -- invis
    info[toon .. 'ivu'] = ''
    info[toon .. 'inv'] = ''
    info[toon .. 'movement'] = ''
    info[toon .. 'levi'] = ''
  
    
    -- ivu  
    if mq.TLO.DanNet(toon).Observe('Me.SPA[315]')() == '0' and mq.TLO.DanNet(toon).Observe('Me.SPA[28]')() == '0' then
      info[toon .. 'ivu'] = ''
    else 
      info[toon .. 'ivu'] = 'U'
    end

    -- ivu  
    if mq.TLO.DanNet(toon).Observe('Me.SPA[314]')() == '0' and mq.TLO.DanNet(toon).Observe('Me.SPA[12]')() == '0' then
      info[toon .. 'inv'] = ''
    else
      info[toon .. 'inv'] = 'I'
    end

    -- movement  
    if mq.TLO.DanNet(toon).Observe('Me.SPA[3]')() == '0'  then
      info[toon .. 'movement'] = ''
    else
      info[toon .. 'movement'] = 'M'
    end

    -- Levi  
    if mq.TLO.DanNet(toon).Observe('Me.SPA[57]')() == '0'  then
      info[toon .. 'levi'] = ''
    else
      info[toon .. 'levi'] = 'L'
    end

  ::continue::  
  end

end












local function imguicallback()
  driveinfo()
  openGUI, shouldDrawHUD = ImGui.Begin('Drive###EntropyDrive', openGUI)
  
  local toon = tempname
  
  if shouldDrawHUD and (info['build'] == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then

  elseif shouldDrawHUD then  
      
    -- column 1
    ImGui.Columns(7, 'tooninfo', false)
      for i = 1, mq.TLO.DanNet.PeerCount(info['channel'])() 
      do
        toon = mq.TLO.DanNet.Peers(info['channel']).Arg(i,'|')()
        if mq.TLO.Spawn(toon).ID() == 0 then 
          goto continue 
        end

        -- dead
        if info[toon .. 'state'] == 0 then
          ImGui.TextColored(1, 0, 0, 1, toon)
          goto continue
        -- auto on
        elseif info[toon .. 'state'] == 1 then
          ImGui.TextColored(0, 1, 0, 1, toon)
        -- auto off
        elseif info[toon .. 'state'] == 2 then
          ImGui.TextColored(0.39, 0.58, 0.92, 1, toon)
        -- mac not running
        elseif info[toon .. 'state'] == 3 then
          ImGui.TextColored(0.90, 0.20, 0.92, 1, toon)
        end
        ::continue::       
      end


    -- column 2
    ImGui.NextColumn()

      for i = 1, mq.TLO.DanNet.PeerCount(info['channel'])() 
      do      
        toon = mq.TLO.DanNet.Peers(info['channel']).Arg(i,'|')()

        if mq.TLO.Spawn(toon).ID() == 0 then 
          goto continue 
        end
        
        ImGui.TextColored(0.39, 1, 0.92, 1, info[toon .. 'ivu'])
        ImGui.SameLine()
        ImGui.TextColored(0.39, 1, 0.92, 1, info[toon .. 'inv'])
        ImGui.SameLine()
        -- ImGui.TextColored(0.39, 1, 0.92, 1, info[toon .. 'movement'])
        ImGui.SameLine()
        ImGui.TextColored(0.39, 1, 0.92, 1, info[toon .. 'levi'])
        ::continue::
      end


    -- column 3
    ImGui.NextColumn()
    
      for i = 1, mq.TLO.DanNet.PeerCount(info['channel'])() 
      do
        toon = mq.TLO.DanNet.Peers(info['channel']).Arg(i,'|')()
      
        if mq.TLO.Spawn(toon).ID() == 0 then 
          goto continue 
        end
        ImGui.TextColored(0.39, 0.58, 0.92, 1, info[toon .. 'distance'])
        ImGui.SameLine()
        ImGui.TextColored(0, 1, 1, .9, info[toon .. 'casting'])

        ::continue::
      end


    -- column 4
    ImGui.NextColumn()
     if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
        cmd_button('Auto: on', 60, 22, 'env auto')
      else 
        cmd_button('Auto: off', 60, 22, 'env auto')
      end
      cmd_button('On', 60, 22, 'on')
      cmd_button('Off', 60, 22, 'off')
      cmd_button('Invis', 60, 22, 'invis')


    -- column 5
    ImGui.NextColumn()
      cmd_button('Tie: Nav', 60, 22, 'dga /tie nav')
      cmd_button('Tie: Stick', 60, 22, 'dga /tie stick')
      cmd_button('Tie', 60, 22, 'dga /tie')


    -- column 6
    ImGui.NextColumn()
      cmd_button('Control', 60, 22, 'env incharge')
      cmd_button('Gather', 60, 22, 'gather')
      cmd_button('Here', 60, 22, 'here')
      cmd_button('Int Pull', 60, 22, 'pull one int')


    -- column 7
    ImGui.NextColumn()
      -- all
      cmd_button_all('I', 15, 22, 'autoinventory')

      -- kills      
      cmd_button_all('Inv', 30, 22, 'nomore invis')
      ImGui.SameLine()
      cmd_button_all('Lev', 30, 22, 'nomore levi')
      
      -- hail/assist
      cmd_button('Assist', 60, 22, 'dge /assist ' .. mq.TLO.Me.DisplayName())
      cmd_button('Hail', 60, 22, 'dga /hail')
      
     

    ImGui.Columns()
  end
  ImGui.End()
end

mq.imgui.init('drivewindow', imguicallback)

while openGUI do 
  mq.delay(1000)
end







 



