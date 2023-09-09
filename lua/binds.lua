--
-- entropoy.mac
-- binds.lua
-- 
--
--


require 'common'
local sql = require('lsqlite3')








local admin2 = function (...)
  local op = {...}

  if op[1] == 'members' then
    local basicTypes = {'Character', 'Target', 'Spawn'}
    local memberCount = 1

    for _key, type in pairs(basicTypes) do
       local index = 0
       while mq.TLO.Type(type:lower()).Member(index)() do
          local memberName = mq.TLO.Type(type:lower()).Member(index)()
          local memberValue = tostring(mq.TLO.Me[memberName]())
          print(string.format('%d - %s (%s)', memberCount, memberName, memberValue))
          index = index + 1
          memberCount = memberCount + 1
       end
    end
  end
  
  if op[1] == 'spawn' then
    _spawn = mq.TLO.Target
    print(_spawn.Type())
    for _, _spawn in ipairs(mq.getFilteredSpawns(filter)) do
      print(_spawn.DisplayName() .. ' :: distance: ' .. round(_spawn.Distance(), 0))
    end
  end
  
  
end

mq.bind('/admin2', admin2)




function finis (...)
  local op = {...}
  
  if op[1] ~= nil then  
    out(2, '/finis', op[1])
  else
    out(2, '/finis', '[all, cast, move, combat]')
  end
  
  
  -- stop everything possible  
  if op[1] == 'all' then
    control_combat('stop_all')      
    control_movement('stop_all')
    control_casting('stop_all')
    control_target('release')
    
    
    
  -- stop all casting
  elseif op[1] == 'cast' then
    control_casting('stop_all')

  -- stop all movement
  elseif op[1] == 'move' then
    control_movement('stop_all')

  -- stop all combat
  elseif op[1] == 'combat' then
    control_combat('stop_all')

  -- release target
  elseif op[1] == 'target' then
    control_target('release')


  end
  
end

mq.bind('/finis', finis)






function move (...)
  local op = {...}

  if op[1] == nil then  
    out(2, '/move', '[stop, stand, sit, duck, jump, rewind, loc y x, to name or id]')
  end
  
  -- duck, duck, goose?
  if op[1] == 'duck' then
    if mq.TLO.Me.State() ~= 'duck' then
     if mq.TLO.Me.Mount.ID() ~= nil then
        mq.cmd.dismount()
      end
      if mq.TLO.Me.State() ~= 'stand' then
        mq.cmd.stand()
      end
      -- mq.cmd.stand()     
      mq.delay(800)
      mq.cmd.keypress('duck')
      mq.delay(100)
      mq.cmd.keypress('forward')
      out(2, '/move', 'duck')
    end

  -- sit down
  elseif op[1] == 'sit' then
    if mq.TLO.Me.State() ~= 'sit' then
      mq.cmd.sit()
      out(2, '/move', 'sit')
    end

  -- stand up
  elseif op[1] == 'stand' then
    if mq.TLO.Me.State() ~= 'stand' then
      mq.cmd.stand()
      out(2, '/move', 'stand')
    end
  
  -- jump, cause why not
  elseif op[1] == 'jump' then
    mq.cmd.keypress('jump')
    out(2, '/move', 'jump')
    
  -- rewind position
  elseif op[1] == 'rewind' then
    mq.cmd.rewind()
    out(2, '/move', 'rewind')
  -- stop all movement
  elseif op[1] == 'stop' then
    control_movement('stop')
    out(2, '/move', 'stop')
    


  -- move to something
  elseif op[1] == 'to' then

    local navlog = mq.TLO.Macro.Variable('maEntropy').Find('swNavLogSpew').Value()

    if mq.TLO.Target.ID() ~= 0 and op[2] == nil then
      
      local _spawn = mq.TLO.Target
      local melee_distance = math.abs(_spawn.MaxRangeTo() * tonumber(string.format(".%.1d", mq.TLO.Macro.Variable('maHard').Find('stMaxMeleeAdj').Value())))
      out(2, '/move to', _spawn.DisplayName())
      mq.cmd.nav('id ' .. _spawn.ID() .. '|dist=' .. melee_distance .. ' log=off')




    elseif op[2] ~= nil then
      -- error check
      if mq.TLO.Spawn(op[2]).ID() <= 1 then
        out(2.1, '/move to', op[2], 'invalid spawn ID')
        return
      end
      local _spawn = mq.TLO.Spawn(op[2])
      local melee_distance = math.abs(_spawn.MaxRangeTo() * tonumber(string.format(".%.1d", mq.TLO.Macro.Variable('maHard').Find('stMaxMeleeAdj').Value())))


      if mq.TLO.Navigation.PathExists('id ' .. _spawn.ID())() == true then
        out(2, '/move to', _spawn.DisplayName())
        mq.cmd.nav('id ' .. _spawn.ID() .. '|dist=' .. melee_distance .. ' log='..navlog)
      else
        out(2, '/move to', _spawn.DisplayName().. ' \arno valid path\ax')
      end

    end

 
  -- move to location
  elseif op[1] == 'loc' then
    -- error check
    if op[2] == nil then
      op[2] = '?'
    elseif op[3] == nil then
      op[3] = '?'
    end
    if op[2] == '?' or op[3] == '?' then
      out(2, '/move loc \arError: \axy=' .. op[2] .. ' x=' .. op[3])
      return
    end
    print(outs['move'] ..'LOC \a-ty:\ax' .. op[2] .. ' \a-tx\ax' .. op[3])
    mq.cmd.nav('locyxz ' .. op[2] .. ' ' .. op[3] .. ' |dist=6 log='..•navlog)
    
  end
end
    
mq.bind('/move', move)








local running = true
while running do
    if mq.TLO.EverQuest.GameState() == 'INGAME' then
      mq.delay(10)
    else
      local running = false
    end
end



function filter(_spawn)
  return _spawn.Type() == 'NPC'
end 



