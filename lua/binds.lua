--
-- entropoy.mac
-- binds.lua
-- 
--
--


require 'common'
local sql = require('lsqlite3')


local admin2 = function (...)
  local option = {...}

  if option[1] == 'members' then
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
  
  if option[1] == 'spawn' then
    _spawn = mq.TLO.Target
    print(_spawn.Type())
    for _, _spawn in ipairs(mq.getFilteredSpawns(filter)) do
      print(_spawn.DisplayName() .. ' :: distance: ' .. round(_spawn.Distance(), 0))
    end
  end
  
  
end

mq.bind('/admin2', admin2)




function filter(_spawn)
  return _spawn.Type() == 'NPC'
end 









move = function (...)
  local option = {...}

  -- duck, duck, goose?
  if option[1] == 'duck' then
    if mq.TLO.Me.State() ~= 'duck' then
      if mq.TLO.Me.Mount.ID() ~= nil then
        mq.cmd('/dismount')
      end
      mq.cmd('/stand')     
      mq.delay(800)
      mq.cmd('/keypress duck')
    end

  -- sit down
  elseif option[1] == 'sit' then
    if mq.TLO.Me.State() ~= 'sit' then
      mq.cmd('/sit')
    end

  -- stand up
  elseif option[1] == 'stand' then
    if mq.TLO.Me.State() ~= 'stand' then
      mq.cmd('/stand')
    end
  
  -- jump, cause why not
  elseif option[1] == 'jump' then
    mq.cmd('/keypress jump')
    
  -- rewind position
  elseif option[1] == 'rewind' then
    mq.cmd('/rewind')

  -- stop all movement
  elseif option[1] == 'stop' then
    print(outs['move'] .. 'Stop')

    if mq.TLO.Navigation.Active() == true then
      mq.cmd('/nav stop |log=off')
    end
    if mq.TLO.Stick.Active() == 'TRUE' then
      mq.cmd('/squelch /stick off')
    end
    if mq.TLO.Me.Moving() == 'TRUE' then
      mq.cmd('/keypress forward')
      mq.cmd('/keypress back')
    end
    
    




  -- move to something
  elseif option[1] == 'to' then
    if mq.TLO.Target.ID() ~= 0 and option[2] == nil then
      local _spawn = mq.TLO.Target
      print(math.abs(_spawn.MaxRangeTo() + mq.TLO.Macro.Variable('maHard').Find('stMaxMeleeAdj').Value()))
      print(outs['move'] .. _spawn.DisplayName())
      mq.cmd('/nav id ' .. _spawn.ID() .. '|dist=' .. math.abs(_spawn.MaxRangeTo() + mq.TLO.Macro.Variable('maHard').Find('stMaxMeleeAdj').Value()) .. ' log=off')
    elseif option[2] ~= nil then
      -- error check
      if mq.TLO.Spawn(option[2]).ID() <= 1 then
        print(outs['move'] .. option[2] .. ' [\arspawn invalid\ax]')
        return
      end
      local _spawn = mq.TLO.Spawn(option[2])
      if mq.TLO.Navigation.PathExists('id ' .. _spawn.ID())() == true then
        print(outs['move'] .. _spawn.DisplayName())
        mq.cmd('/nav id ' .. _spawn.ID() .. '|dist=' .. math.abs(_spawn.MaxRangeTo() + mq.TLO.Macro.Variable('maHard').Find('stMaxMeleeAdj').Value()) .. ' log=off')
      else
        print(outs['move'] .. _spawn.DisplayName() .. outs['nopath'])
      end

    end

 
  -- move to location
  elseif option[1] == 'loc' then
    -- error check
    if option[2] == nil then
      option[2] = '?'
    elseif option[3] == nil then
      option[3] = '?'
    end
    if option[2] == '?' or option[3] == '?' then
      print('\arError: \axy=' .. option[2] .. ' x=' .. option[3])
      return
    end
    print(outs['move'] ..'LOC \a-ty:\ax' .. option[2] .. ' \a-tx\ax' .. option[3])
    mq.cmd('/nav locyxz ' .. option[2] .. ' ' .. option[3] .. ' |dist=6 log=' .. mq.TLO.Macro.Variable('maEntropy').Find('swNavLogSpew').Value())
    
  else
    
    
    
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




