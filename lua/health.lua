--[[ 

testing group health return
  
--]]




require "common"

HURT = {
  spawn = mq.TLO.Me,
  pct = 100,
  id = 0,
  healpoint = 100,
  range = mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value(),
  count = 0,
  PctHO = mq.TLO.Macro.Variable('tmpHealAdjust')()
}



-- health lower?
function healthCheck (memberPct)
  --print('PC HP:' .. memberPct)
  --print('healPoint: ' .. mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..HURT.spawn.Class.ShortName()).Value())
  if memberPct <= tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..HURT.spawn.Class.ShortName()).Value()) then
    return true
  else 
    return false
  end
end



while true
do

  HURT.count = 0
  --mq.cmd.varset('hurtCount', 0)
  --HURT.pct = 0
  --HURT.id = 0
  --mq.cmd.varset('hurtData', 0)
       
         
  for i = 0, mq.TLO.Group.Members() do

    if i == 0 then
      HURT.spawn = mq.TLO.Me
    else
      HURT.spawn = mq.TLO.Group.Member(i)
    end
    -- print('Group member: ' .. i .. ' ' .. HURT.spawn.DisplayName() .. ' ' .. HURT.spawn.PctHPs())
    -- print('Group member present: ' .. i .. ' ' .. mq.TLO.Group.Member(i).Present())

    if HURT.spawn.Dead() ~= 1 and mq.TLO.Group.Member(i).ID() ~= 0 and rangeCheck(HURT.spawn.Distance(), HURT.range) and healthCheck(HURT.spawn.PctHPs()) then
      --print('here ' .. HURT.pct)
      if HURT.spawn.PctHPs() <= HURT.pct then
        HURT.pct = HURT.spawn.PctHPs()
        HURT.id = HURT.spawn.ID()
        HURT.count = HURT.count + 1
        --print('Group member: ' .. i .. ' ' .. HURT.spawn.DisplayName() .. ': ' .. '(' .. mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..HURT.spawn.Class.ShortName()).Value() .. ')' .. HURT.spawn.PctHPs())
      end
    end
    
    mq.delay(50)
  
  end
  

  --for i = 1, mq.TLO.Me.XTargetSlots() do
  --  HURT.spawn = mq.TLO.Me.XTarget(i)
  --end
  
  
  
  
  
  

  -- set hurtData2
  if HURT.count == 0 then
    mq.cmd.varset('hurtData', 0)
    mq.cmd.varset('hurtCount', 0)
    HURT.pct = 100
    HURT.id = 0
  else 
     mq.cmd.vardata('hurtData', 'Spawn['..HURT.id..']')
     mq.cmd.varset('hurtCount', HURT.count)
  end


end









--mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..mq.TLO.Group.Member(i).Class.ShortName()).Value()



