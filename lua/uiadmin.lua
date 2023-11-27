--
-- entropoy.mac
-- uiadmin.lua
-- 
-- ui administration
--



require 'common'
local openGUI = true
local shouldDrawHUD = true

local function imguicallback()
  hudInfo()
  
  openGUI, shouldDrawHUD = ImGui.Begin('uiadmin###EntropyUIAdmin', openGUI)
  
  if shouldDrawHUD and ent.build == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()

  elseif shouldDrawHUD then  
    ImGui.NewLine() 
    cmd_button('hud', 60, 22, 'ui2 hud on')
    ImGui.NewLine()  

  -- autostart
    if ImGui.CollapsingHeader('autostart') then
      indent(1,1) 
      ImGui.NewLine()  
      edit_switch_perm('Main', 'maEntropy', 'swHUDAuto')
      edit_switch_perm('Drive', 'maEntropy', 'swHUDDriveAuto')
      edit_switch_perm('Event Buttons', 'maHud', 'swHUDEventButtonsAuto')
      edit_switch_perm('Radar', 'maHud', 'stRadarAuto')



      ImGui.NewLine()  
      indent(1,2)
    end
    
  -- HUD
    if ImGui.CollapsingHeader('hud') then
      indent(1,1)
      ImGui.NewLine()
      edit_switch_perm('show buttons on hud', 'maHud', 'swHUDbuttons')
      edit_switch_perm('Main Target: On for display name, OFF for real name', 'maHud', 'swHUDDisplayName')
      edit_text_perm('blank', 'maHud', 'stHUDBlank')
          
      ImGui.NewLine()
      indent(1,2)

      if ImGui.TreeNode('tabs') then
        indent(1,1) 
        ImGui.NewLine()  
        ImGui.Columns(3, 'hud', false)
          edit_switch_perm('buff', 'maHud', 'swTabBuff')
          edit_switch_perm('heal', 'maHud', 'swTabHeal')
          edit_switch_perm('combat', 'maHud', 'swTabCombat')
          edit_switch_perm('pull', 'maHud', 'swTabPull')
          edit_switch_perm('about', 'maHud', 'swTabAbout')
        ImGui.NextColumn()
          edit_switch_perm('rez', 'maHud', 'swTabRez')
          edit_switch_perm('tie', 'maHud', 'swTabTie')
          edit_switch_perm('rest', 'maHud', 'swTabRest')
          edit_switch_perm('target', 'maHud', 'swTabTarget')
        ImGui.NextColumn()
        ImGui.Columns()  
        ImGui.NewLine()  
        indent(1,2)
      ImGui.TreePop() 
      end

      if ImGui.TreeNode('buttons') then
        ImGui.Text('* no more than 12 is suggested')
        indent(1,1) 
        ImGui.Columns(3, 'hudbuttons', false)
          edit_switch_perm('all auto', 'maHud', 'swHUDBTNallauto')
          edit_switch_perm('all manual', 'maHud', 'swHUDBTNallmanual')
          edit_switch_perm('auto', 'maHud', 'swHUDBTNauto')
          edit_switch_perm('tie switch', 'maHud', 'swHUDBTNtie')
          edit_switch_perm('tie nav', 'maHud', 'swHUDBTNtienav')
          edit_switch_perm('tie stick', 'maHud', 'swHUDBTNtiestick')
        ImGui.NextColumn()
          edit_switch_perm('incharge', 'maHud', 'swHUDBTNincharge')
          edit_switch_perm('stack invis', 'maHud', 'swHUDBTNinvis')
          edit_switch_perm('no invis', 'maHud', 'swHUDBTNnoinvis')
          edit_switch_perm('int pull', 'maHud', 'swHUDBTNintpull')
          edit_switch_perm('gather', 'maHud', 'swHUDBTNgather')
          edit_switch_perm('here', 'maHud', 'swHUDBTNhere')
        ImGui.NextColumn()
          edit_switch_perm('ui: radar', 'maHud', 'swHUDBTNradar')
          edit_switch_perm('ui: drive', 'maHud', 'swHUDBTNdrive')
          edit_switch_perm('ui: event buttons', 'maHud', 'swHUDBTNevent')
          edit_switch_perm('ui: notes', 'maHud', 'swHUDBTNnotes')
        ImGui.Columns()  
        ImGui.NewLine()  
        indent(1,2)
      ImGui.TreePop() 
      end


      if ImGui.TreeNode('headder') then
        indent(1,1) 
        ImGui.NewLine()  
        ImGui.Columns(3, 'hud', false)
          edit_switch_perm('Entropy', 'maHud', 'swTitleEnt')
          edit_switch_perm(ent['build'], 'maHud', 'swTitleBuild')
          edit_switch_perm('Name', 'maHud', 'swTitleName')
          edit_switch_perm('Auto / Manual', 'maHud', 'swTitleAM')
        ImGui.NextColumn()
          edit_switch_perm('INV', 'maHud', 'swTitleIV')
          edit_switch_perm('IVU', 'maHud', 'swTitleIVU')
          edit_switch_perm('Adj', 'maHud', 'swTitleHealAdj')
        ImGui.NextColumn()
        ImGui.Columns()  
      indent(1,2)
      ImGui.TreePop() 
      end
    ImGui.NewLine()
    end


  -- drive
    if ImGui.CollapsingHeader('drive') then
      indent(1,1) 
      ImGui.NewLine()  
      edit_switch_perm('Auto start drive on mac start', 'maEntropy', 'swHUDDriveAuto')
      -- edit_switch_perm('close drive hud macro end', 'maEntropy', 'swHUDDriveCloseonEnd')
      edit_switch_perm('show buttons on drive', 'maHud', 'swHUDDrivebuttons')

      ImGui.NewLine()
      indent(1,2)
    end

  -- notes
    if ImGui.CollapsingHeader('notes') then
      indent(1,1) 
      ImGui.NewLine()  

      ImGui.NewLine()
      indent(1,2)
    end

  -- radar
    if ImGui.CollapsingHeader('radar') then
      indent(1,1) 
      ImGui.NewLine()  
      
        -- type of radar to view
        local current_view = mq.TLO.Macro.Variable('maHud').Find('stRadarViewType').Value()
        if ImGui.BeginCombo("view", current_view) then
          for _, t in ipairs({ 'spawn', 'xtarget' }) do
            local selected_view = t == current_view
            if ImGui.Selectable(t, selected_view) and not selected_view then
              mq.cmd.luaedit('stRadarViewType', '"'..t..'"')
            end
          end
          ImGui.EndCombo()
        end

        
        -- spawn count       
        if mq.TLO.Macro.Variable('maHud').Find('stRadarViewType').Value() == 'spawn' then
          ImGui.NewLine()  
          local current_sorttype = mq.TLO.Macro.Variable('maHud').Find('stRadarSortType').Value()
          if ImGui.BeginCombo("sort", current_sorttype) then
            for _, t in ipairs({ 'npc', 'pc' }) do
              local selectedshade = t == current_sorttype
              if ImGui.Selectable(t, selectedshade) and not selectedshade then
                mq.cmd.luaedit('stRadarSortType', '"'..t..'"')
              end
            end
            ImGui.EndCombo()
          end
        
          local radarcount = tonumber(mq.TLO.Macro.Variable('maHud').Find('stRadarCount').Value())
          radarcount, used = ImGui.DragInt("spawncount##radarcount", radarcount, 1, 0, 1000);
          if used then
            mq.cmd.luaedit('stRadarCount', radarcount)  
          end

          edit_switch_perm('targetable?', 'maHud', 'swRadarSortTargetable')      
        end     

      ImGui.NewLine()
      edit_switch_perm('pet button', 'maHud', 'swRadarshowPetbutton')      
      edit_switch_perm('mark buttons', 'maHud', 'swRadarshowMarkbutton')
      ImGui.NewLine()
      indent(1,2)
    end



  -- build
    if ImGui.CollapsingHeader('build') then
      indent(1,1) 
      ImGui.NewLine()  

      ImGui.NewLine()
      indent(1,2)
    end

  end
  ImGui.End()

end

mq.imgui.init('uiadminwindow', imguicallback)

while openGUI do 
  mq.delay(1000) 
end







 



