--
-- entropy.mac
-- 
-- 
-- 
--



require 'common'
local openGUI = true
local shouldDrawHUD = true




local function imguicallback()
  hudInfo()
  
  openGUI, shouldDrawHUD = ImGui.Begin('debug###EntropyDebug', openGUI)
  
  if shouldDrawHUD and ent['build'] == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    ImGui.NewLine() 


    -- DEBUG
    if ImGui.CollapsingHeader('/debug') then
      ImGui.NewLine()
 
      if ImGui.TreeNode('buff') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugbuff', false)
          edit_switch_temp('cb', 'maDebug', 'cb')
          edit_switch_temp('ammo', 'maDebug', 'ammo')
          edit_switch_temp('beg', 'maDebug', 'beg')
          edit_switch_temp('unity', 'maDebug', 'unity')
        ImGui.NextColumn()
          edit_switch_temp('aura', 'maDebug', 'aura')
          edit_switch_temp('now', 'maDebug', 'now')
          edit_switch_temp('crew', 'maDebug', 'crew')
          edit_switch_temp('buffself', 'maDebug', 'buffself')
        ImGui.NextColumn()
          edit_switch_temp('shrink', 'maDebug', 'shrink')
          edit_switch_temp('poison', 'maDebug', 'poison')
          edit_switch_temp('class', 'maDebug', 'class')
          edit_switch_temp('temp', 'maDebug', 'temp')
        ImGui.NextColumn()
          edit_switch_temp('blocked', 'maDebug', 'blocked')
          edit_switch_temp('buffgroup', 'maDebug', 'buffgroup')
          edit_switch_temp('buffraid', 'maDebug', 'buffraid')
          edit_switch_temp('drop', 'maDebug', 'drop')
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  
      
      if ImGui.TreeNode('heal') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugheal', false)
          --cure
          edit_switch_temp('cure', 'maDebug', 'cure')
          --xt
          edit_switch_temp('xt', 'maDebug', 'xt')
          --group
          edit_switch_temp('group', 'maDebug', 'group')
        ImGui.NextColumn()
          --pet
          edit_switch_temp('pet', 'maDebug', 'pet')
          --self
          edit_switch_temp('self', 'maDebug', 'self')
          --hurt
          edit_switch_temp('hurt', 'maDebug', 'hurt')
        ImGui.NextColumn()
          --tot
          edit_switch_temp('tot', 'maDebug', 'tot')
          --dannet
          edit_switch_temp('dannet', 'maDebug', 'dannet')
          --rez
          edit_switch_temp('rez', 'maDebug', 'rez')
        ImGui.NextColumn()
          --chain
          edit_switch_temp('chain', 'maDebug', 'chain')
          --ch
          edit_switch_temp('ch', 'maDebug', 'ch')
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('home') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debughome', false)
          edit_switch_temp('face', 'maDebug', 'face')
          edit_switch_temp('tie', 'maDebug', 'tie')
          edit_switch_temp('stick', 'maDebug', 'stick')
        ImGui.NextColumn()
          edit_switch_temp('stop', 'maDebug', 'stop')
          edit_switch_temp('home', 'maDebug', 'home')
          edit_switch_temp('gtfo', 'maDebug', 'gtfo')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('minion') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugminion', false)
          edit_switch_temp('petbuild', 'maDebug', 'petbuild')
          edit_switch_temp('lost', 'maDebug', 'lost')
          edit_switch_temp('sic', 'maDebug', 'sic')
        ImGui.NextColumn()
          edit_switch_temp('temp', 'maDebug', 'temp')
          edit_switch_temp('heal', 'maDebug', 'heal')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('debuff') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugdebuff', false)
          edit_switch_temp('tash', 'maDebug', 'tash')
          edit_switch_temp('malo', 'maDebug', 'malo')
          edit_switch_temp('mez', 'maDebug', 'mez')
        ImGui.NextColumn()
          edit_switch_temp('slow', 'maDebug', 'slow')
          edit_switch_temp('eradicate', 'maDebug', 'eradicate')
          edit_switch_temp('snare', 'maDebug', 'snare')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('cast') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugcast', false)
          edit_switch_temp('item', 'maDebug', 'item')
          edit_switch_temp('nuke', 'maDebug', 'nuke')
          edit_switch_temp('dot', 'maDebug', 'dot')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('loot') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugloot', false)
          edit_switch_temp('loot', 'maDebug', 'loot')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('mode') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugmode', false)
          edit_switch_temp('forage', 'maDebug', 'forage')
          edit_switch_temp('drag', 'maDebug', 'drag')
          edit_switch_temp('summon', 'maDebug', 'summon')
        ImGui.NextColumn()
          edit_switch_temp('petfarm', 'maDebug', 'petfarm')
          edit_switch_temp('harvest', 'maDebug', 'harvest')
          edit_switch_temp('hunt', 'maDebug', 'hunt')
        ImGui.NextColumn()
          edit_switch_temp('fish', 'maDebug', 'fish')
          edit_switch_temp('farm', 'maDebug', 'farm')
          edit_switch_temp('lush', 'maDebug', 'lush')
        ImGui.NextColumn()
          edit_switch_temp('trainskill', 'maDebug', 'trainskill')
          edit_switch_temp('trainspell', 'maDebug', 'trainspell')
          edit_switch_temp('temppet', 'maDebug', 'temppet')
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('combat') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugcombat', false)
          edit_switch_temp('def', 'maDebug', 'def')
          edit_switch_temp('target', 'maDebug', 'target')
          edit_switch_temp('melee', 'maDebug', 'melee')
          edit_switch_temp('burn', 'maDebug', 'burn')
        ImGui.NextColumn()
          edit_switch_temp('agro', 'maDebug', 'agro')
          edit_switch_temp('pull', 'maDebug', 'pull')
          edit_switch_temp('misc', 'maDebug', 'misc')
          edit_switch_temp('assist', 'maDebug', 'assist')
        ImGui.NextColumn()
          edit_switch_temp('decision', 'maDebug', 'decision')
          edit_switch_temp('proc', 'maDebug', 'proc')
          edit_switch_temp('ttl', 'maDebug', 'ttl')
          edit_switch_temp('sash', 'maDebug', 'sash')
        ImGui.NextColumn()
          edit_switch_temp('status', 'maDebug', 'status')
          edit_switch_temp('wait', 'maDebug', 'wait')
          edit_switch_temp('hardstop', 'maDebug', 'hardstop')
          edit_switch_temp('prediction', 'maDebug', 'prediction')
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('other') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugother', false)
          edit_switch_temp('rest', 'maDebug', 'rest')
          edit_switch_temp('meal', 'maDebug', 'meal')
          edit_switch_temp('spire', 'maDebug', 'spire')
          edit_switch_temp('glyph', 'maDebug', 'glyph')
          edit_switch_temp('fireworks', 'maDebug', 'fireworks')
        ImGui.NextColumn()
          edit_switch_temp('manarecover', 'maDebug', 'manarecover')
          edit_switch_temp('fade', 'maDebug', 'fade')
          edit_switch_temp('event', 'maDebug', 'event')
          edit_switch_temp('watch', 'maDebug', 'watch')
        ImGui.NextColumn()
          edit_switch_temp('tag', 'maDebug', 'tag')
          edit_switch_temp('safe', 'maDebug', 'safe')
          edit_switch_temp('intensity', 'maDebug', 'intensity')
          edit_switch_temp('chrwatch', 'maDebug', 'chrwatch')
        ImGui.NextColumn()
          edit_switch_temp('staminarecover', 'maDebug', 'staminarecover')
          edit_switch_temp('inventoryscan', 'maDebug', 'inventoryscan')
          edit_switch_temp('invis', 'maDebug', 'invis')
          edit_switch_temp('gather', 'maDebug', 'gather')
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('admin') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugadmin', false)
          edit_switch_temp('build', 'maDebug', 'build')
          edit_switch_temp('observe', 'maDebug', 'observe')
          edit_switch_temp('house', 'maDebug', 'house')
          edit_switch_temp('exit', 'maDebug', 'exit')
        ImGui.NextColumn()
          edit_switch_temp('sql', 'maDebug', 'sql')
          edit_switch_temp('edit', 'maDebug', 'edit')
          edit_switch_temp('e3', 'maDebug', 'e3')
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('schema') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugschema', false)
          edit_switch_temp('init', 'maDebug', 'init')
          edit_switch_temp('s_zone', 'maDebug', 's_zone')
          edit_switch_temp('s_character', 'maDebug', 's_character')
          edit_switch_temp('s_loot', 'maDebug', 's_loot')
        ImGui.NextColumn()
          edit_switch_temp('s_nav', 'maDebug', 's_nav')
          edit_switch_temp('s_shared', 'maDebug', 's_shared')
          edit_switch_temp('s_stale', 'maDebug', 's_stale')
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  

      if ImGui.TreeNode('area') then
        ImGui.NewLine()
        ImGui.Columns(4, 'debugarea', false)
          edit_switch_temp('area', 'maDebug', 'area')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
      end  
      ImGui.NewLine()

    end

    -- castData
    if ImGui.CollapsingHeader('spellData') then
      indent(1,1)
      
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'checking:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('spellData').RankName())
      
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'id:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('spellData').ID())
      
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'type:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maCD').Find('type').Value())

      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'ready:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maCD').Find('ready').Value())

      ImGui.NewLine()
      indent(1,2)
    end

    -- hurtData
    if ImGui.CollapsingHeader('hurtData') then
      indent(1,1)
      
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'name:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtData').DisplayName())
      ImGui.SameLine()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, '[')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtData').ID())
      ImGui.SameLine()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtData').Class.ShortName())
      ImGui.SameLine()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, ']')
      
      ImGui.Columns(2, 'hurtdat', false)

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'hurtCount:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtCount')())

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'hurtDead:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtDead')())

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'range:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtData').Distance())
      
      ImGui.NextColumn()

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'PctHps:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('hurtData').PctHPs())

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'heal point:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..mq.TLO.Macro.Variable('hurtData').Class.ShortName()).Value())

      ImGui.Columns()
      ImGui.NewLine()
      indent(1,2)
    end

    -- targetData
    if ImGui.CollapsingHeader('targetData') then
      indent(1,1)

      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'name: ')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('targetData').Name() or 0)
      ImGui.SameLine()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, '[')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('targetData').ID() or 0)
      ImGui.SameLine()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, ']')

      -- ImGui.Columns(2, 'targetdata', false)

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'engage pct:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maCC').Find('stPctEngage').Value() or 0)

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'los:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, tostring(mq.TLO.Macro.Variable('targetData').LineOfSight()) or 0)

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'distance:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('targetData').Distance() or 0)

      -- ImGui.NextColumn()

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'PctHPs:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('targetData').PctHPs() or 0)

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'body:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('targetData').Body() or 0)

        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'animation:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('targetData').Animation() or 0)

      -- ImGui.Columns()

      ImGui.NewLine()
      indent(1,2)
    end

    -- assistData
    if ImGui.CollapsingHeader('assistData') then
      indent(1,1)

      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'assist set:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maCC').Find('stAssistMode').Value() or '--')
      
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'name:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('assistData').DisplayName() or '--')


      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'id:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('assistData').ID() or 0)
      
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'distance:')
      ImGui.SameLine()
      ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('assistData').Distance() or 0)


      ImGui.NewLine()
      indent(1,2)
    end

    -- target data
    if ImGui.CollapsingHeader('target data') then
      indent(1,1)
      
      if mq.TLO.Target.ID() then
        ImGui.NewLine()

        -- ID
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'ID:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Target.ID() or 0)        
        -- name
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Name:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Target.Name() or '--')        
        -- displayname
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'DisplayName:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Target.DisplayName() or '--')        

      
        ImGui.NewLine()    
        
        ImGui.Columns(2, 'noname', false)
  
          -- distance
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'distance:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, round(mq.TLO.Target.Distance(), 2) or 0)   
          
          -- valid loc
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'valid loc:')
          ImGui.SameLine()
          -- ImGui.TextColored(1, 1, 1, 1, mq.TLO.EverQuest.ValidLoc(string.format("%.2f %.2f %.2f", mq.TLO.Target.X(), mq.TLO.Target.Y(), mq.TLO.Target.Z())) or 0)
          

          -- mq.TLO.EverQuest.ValidLoc(string.format("%f %f %f", mq.TLO.Target.X(), mq.TLO.Target.Y(), mq.TLO.Target.Z()))
          
               

        ImGui.NextColumn()

          -- pathexists
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'pathexists:')
          --ImGui.SameLine()
          --ImGui.TextColored(1, 1, 1, 1, mq.TLO.Navigation.PathExists(ID mq.TLO.Target.ID()))

        --pathlength
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'pathlength:')
          --ImGui.SameLine()
          --ImGui.TextColored(1, 1, 1, 1, mq.TLO.Navigation.PathLength(id mq.TLO.Target.ID())
        


        ImGui.Columns()        
        
        
        
        ImGui.NewLine() 
        ImGui.NewLine()    
        ImGui.Columns(2, 'noname', false)

          -- body
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Body:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.Target.Body.Name())        

          -- height
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Height:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, round(mq.TLO.Target.Height(), 2))

          -- height
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'LoS:')
          --ImGui.SameLine()
          
        --ImGui.TextColored(1, 1, 1, 1, mq.TLO.Spawn().LineOfSight())
          -- ImGui.TextColored(1, 1, 1, 1, target['los'])



        ImGui.NextColumn()
        
          -- slowed
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Slowed:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, isSlowed()) 

          -- tashed
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Tashed:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, isTashed()) 

          -- maloed
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Malo:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, isMaloed()) 

          -- crippled
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Cripple:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, isCrippled()) 

          -- snared
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Snared:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, isSnared()) 

          -- ds
          -- ImGui.TextColored(0.39, 0.58, 0.92, 1, 'DS:')
          -- ImGui.SameLine()
          -- ImGui.TextColored(1, 1, 1, 1, target['ds']) 

        ImGui.Columns()
        
      else
        ImGui.NewLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'No Target')
      end
      indent(1,2)
      ImGui.NewLine()
    end
    
    -- local spawns
    if ImGui.CollapsingHeader('local spawns') then
      indent(1,1)
      ImGui.NewLine()
      
      if ImGui.TreeNode("spawn counts in [radius]") then
        
        -- environment radius
        local envrad = mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '/env rad: ['..envrad..'] ')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.SpawnCount('npc radius '..envrad..'')())

        -- pull radius
        local pullrad = mq.TLO.Macro.Variable('maPull').Find('stPullRadius').Value()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '/pull rad: ['..pullrad..'] ')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.SpawnCount('npc radius '..pullrad..'')()) 
        
        -- rez radius
        local rezrad = mq.TLO.Macro.Variable('maRez').Find('stMaxRezRange').Value()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '/rez rad: ['..rezrad..'] ')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.SpawnCount('pccorpse radius '..rezrad..'')()) 

        -- rest radius
        local restrad = mq.TLO.Macro.Variable('maRest').Find('stRestRadius').Value()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '/rest rad: ['..restrad..'] ')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.SpawnCount('npc radius '..restrad..' playerstate 4')()) 

        -- safe radius
        local saferad = mq.TLO.Macro.Variable('maEnv').Find('stEnvSafeRadius').Value()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '/env saferadius: ['..saferad..'] ')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.SpawnCount('npc radius '..saferad..'')()) 


 
        ImGui.TreePop()
      end
      
      ImGui.NewLine()
      

      for x = 1,10,1
      do
        if ImGui.TreeNode(""..x.." :: "..mq.TLO.NearestSpawn(x).DisplayName().."") then

          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'name##:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).Name()) 
          ImGui.SameLine()
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'ID:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).ID()) 

          -- distance
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Distance:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, round(mq.TLO.NearestSpawn(x).Distance(), 2)) 

          -- Y,X,Z     
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Y:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, round(mq.TLO.NearestSpawn(x).Y(), 2)) 
          ImGui.SameLine()
          ImGui.TextColored(0.39, 0.58, 0.92, 1, '  X:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, round(mq.TLO.NearestSpawn(x).X(), 2)) 
          ImGui.SameLine()
          ImGui.TextColored(0.39, 0.58, 0.92, 1, '  Z:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, round(mq.TLO.NearestSpawn(x).Z(), 2)) 
          
          -- Animation
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Animation:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).Animation()) 

          -- Body / type
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Body:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).Body()) 
          ImGui.SameLine()
          ImGui.TextColored(0.39, 0.58, 0.92, 1, '   Type:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).Type()) 

          -- Master / Owner
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Master:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).Master()) 
          ImGui.SameLine()
          ImGui.TextColored(0.39, 0.58, 0.92, 1, '   Owner:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).Owner()) 

          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'LoS:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.NearestSpawn(x).LineOfSight()) 


          ImGui.TreePop()
          ImGui.NewLine()
        end
      end

      indent(1,2)
      ImGui.NewLine()
    end
    
    
  end
  ImGui.End()

end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  mq.delay(1000) 
end







 



