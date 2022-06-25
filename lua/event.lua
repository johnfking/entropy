--
-- entropy.mac
-- 
-- Custom Events
-- 
--



require 'common'
local openGUI = true
local shouldDrawHUD = true




local function imguicallback()
  hudInfo()
  
  openGUI, shouldDrawHUD = ImGui.Begin('/event###EntropyEvent', openGUI)
  
  if shouldDrawHUD and ent['event'] == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    ImGui.NewLine() 


    if ImGui.TreeNode('Options') then
      ImGui.NewLine()
      --edit_text_perm('hide YX', 'maEvent', 'stBloodfallsHideLocYX')
      edit_switch_perm('events active', 'maEntropy', 'swEventsInc')

      ImGui.NewLine()
      indent(1,2)
    end  
    ImGui.NewLine() 



-- Terror of Luclin
    if ImGui.CollapsingHeader('Terror of Luclin 28:ToL') then
      ImGui.NewLine() 
 
      if ImGui.TreeNode('Close the Gates :: Bloodfalls') then
        ImGui.NewLine()
        edit_text_perm('hide YX', 'maEvent', 'st28BloodfallsHideLocYX')
        ImGui.NewLine()
        edit_text_perm('bugs', 'maEvent', 'st28BloodfallsBuggs')
        edit_text_perm('chased', 'maEvent', 'st28BloodfallsChased')
        edit_text_perm('drowning', 'maEvent', 'st28BloodfallsDrowning')
        edit_text_perm('falling', 'maEvent', 'st28BloodfallsFalling')
        edit_text_perm('monster', 'maEvent', 'st28BloodfallsMonster')
        edit_text_perm('curse', 'maEvent', 'st28BloodfallsCurse')
        ImGui.NewLine()
        indent(1,2)
      end  

      if ImGui.TreeNode('Gorgana :: Maidens Eye') then
        ImGui.NewLine()
        edit_text_perm('hide YX', 'maEvent', 'ls28MaidenHideLocYX')
        edit_text_perm('aura', 'maEvent', 'st28MaidenAura')
        ImGui.NewLine()
        indent(1,2)
      end

      if ImGui.TreeNode('Sexy Bitch :: Vex Thal') then
        ImGui.NewLine()
        edit_text_perm('hide YX', 'maEvent', 'st28AetnHideLocYX')
         ImGui.NewLine()
        indent(1,2)
      end  


      if ImGui.TreeNode('Ghost and Golems and Thingies :: Oubliette') then
        ImGui.NewLine()
        --edit_text_perm('hide YX', 'maEvent', 'ls28OublietteHideLocYX')
        edit_text_perm('aura', 'maEvent', 'st28OublietteAura')
        ImGui.NewLine()
        indent(1,2)
      end



      ImGui.NewLine() 
    end



    --if ImGui.CollapsingHeader('Claws of Veeshan') then
    --if ImGui.CollapsingHeader('Torment of Velious') then
    --if ImGui.CollapsingHeader('The Burning Lands') then
    --if ImGui.CollapsingHeader('Ring of Scale') then
    --if ImGui.CollapsingHeader('Empires of Kunark') then
    --if ImGui.CollapsingHeader('The Broken Mirror') then
    --if ImGui.CollapsingHeader('The Darkened Sea') then
    --if ImGui.CollapsingHeader('Call of the Forsaken') then
    --if ImGui.CollapsingHeader('Rain of Fear') then
    --if ImGui.CollapsingHeader('Veil of Alaris') then
    --if ImGui.CollapsingHeader('House of Thule') then
    --if ImGui.CollapsingHeader('Underfoot') then
    --if ImGui.CollapsingHeader('Seeds of Destruction') then
    --if ImGui.CollapsingHeader('Secrets of Faydwer') then
    --if ImGui.CollapsingHeader('The Buried Sea') then
    --if ImGui.CollapsingHeader('The Serpents Spine') then
    --if ImGui.CollapsingHeader('Prophecy of Ro') then
    --if ImGui.CollapsingHeader('Depths of Darkhollow') then
    --if ImGui.CollapsingHeader('Dragons of Norrath') then
    --if ImGui.CollapsingHeader('Omens of War') then
    --if ImGui.CollapsingHeader('Gates of Discord') then
    --if ImGui.CollapsingHeader('Lost Dungeons of Norrath') then
    --if ImGui.CollapsingHeader('The Legacy of Ykesha') then
    --if ImGui.CollapsingHeader('The Planes of Power') then
    --if ImGui.CollapsingHeader('The Shadows of Luclin') then
    --if ImGui.CollapsingHeader('The Scars of Velious') then
    --if ImGui.CollapsingHeader('The Ruins of Kunark') then
 



-- Anniversary events
    if ImGui.CollapsingHeader('Anniversary') then
      ImGui.NewLine()

      if ImGui.TreeNode('Mad Emperor :: Kanor Anniversary Event') then
        edit_text_perm('Hide YX', 'maEvent', 'stKarnorHideLocYX')
        ImGui.NewLine()
        indent(1,2)
      end  
      ImGui.NewLine()


    end
     
    ImGui.NewLine()


  end
  ImGui.End()

end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  mq.delay(1000) 
end







 



