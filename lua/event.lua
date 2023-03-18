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
  
  openGUI, shouldDrawHUD = ImGui.Begin('event###EntropyEvent', openGUI)
  
  if shouldDrawHUD and ent['event'] == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    ImGui.NewLine() 

    if ImGui.TreeNode('Options') then
      ImGui.NewLine()
      edit_switch_perm('events active', 'maEntropy', 'swEventsInc')

      ImGui.NewLine()
      indent(1,2)
    end  
    ImGui.NewLine() 




    if ImGui.CollapsingHeader('Nights of Shadow 29 :: NoS') then
      ImGui.NewLine() 


      -- tier 1
     if ImGui.TreeNode('Paludal Depths :: Insatiable Appetite') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  



      if ImGui.TreeNode('Shar Vahl :: Pit Fight') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  

      if ImGui.TreeNode('Shar Vahl :: Mean Streets') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  


      -- tier 2
      if ImGui.TreeNode('Shadow haven :: When One Door Closes') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  

      if ImGui.TreeNode('Deepshade :: Mycondi Mutiny') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  

      if ImGui.TreeNode('Deepshade :: Dance of the Demiurge') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end 


      -- tier 3
      if ImGui.TreeNode('Darklight Caverns :: The Spirit Fades') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  

      if ImGui.TreeNode('Firefall Pass :: The Shadow Moves') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end  

      if ImGui.TreeNode('Shar Vahl :: Under Seige') then
        ImGui.NewLine()
        
        ImGui.Indent(-20)
      end


      ImGui.NewLine() 



    end

    if ImGui.CollapsingHeader('Terror of Luclin 28 :: ToL') then
      ImGui.NewLine() 
 
      if ImGui.TreeNode('Vader :: Umbral Plains') then
        ImGui.NewLine()
        edit_text_perm('hide YX', 'maEvent', 'ls28UmbralHideLocYX')
        ImGui.NewLine()
        edit_text_perm('home YX', 'maEvent', 'st28UmbralHomeLocYX')
        ImGui.NewLine()
        indent(1,2)
      end  

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

    if ImGui.CollapsingHeader('Claws of Veeshan 27 :: CoV') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Torment of Velious 26 :: ToV') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Burning Lands 25 :: TBL') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Ring of Scale 24 :: ROS') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Empires of Kunark 23 :: EOK') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Broken Mirror 22 :: TBM') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Darkened Sea 21 :: TDS') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Call of the Forsaken 20 :: CoTF') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Rain of Fear 19 :: RoF') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Veil of Alaris 18 :: VoA') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('House of Thule 17 :: HoT') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Underfoot 16 :: UF') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Seeds of Destruction 15 :: SoD') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Secrets of Faydwer 14 :: SoF') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Buried Sea 13 :: TBS') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Serpents Spine 12 :: SS') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Prophecy of Ro 11 :: PoR') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Depths of Darkhollow 10 :: DoD') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Dragons of Norrath 9 :: DoN') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Omens of War 8 :: OOW') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Gates of Discord 7 :: GoD') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('Lost Dungeons of Norrath 6 :: LDoN') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Legacy of Ykesha 5 :: LoY') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Planes of Power 4 :: PoP') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Shadows of Luclin 3 :: SoL') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Scars of Velious 2 :: SoV') then
      ImGui.NewLine() 
    end

    if ImGui.CollapsingHeader('The Ruins of Kunark 1 :: RoK') then
      ImGui.NewLine() 
    end

 



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







 



