--[[ 

testing group health return
  
--]]




require "common"


hurtSpawn = mq.TLO.Me

range = {}
range['envrad'] = mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value()

HURT = {
  pct = 101
  id = 0
  class = CLR
  healpoint = 101
}


-- health lower?
function healthCheck (memberPct)
  --print('PC HP:' .. memberPct)
  --print('healPoint: ' .. mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..hurtSpawn.Class.ShortName()).Value())
  HURT.healpoint = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..hurtSpawn.Class.ShortName()).Value())
  if memberPct <= HURT.healpoint then
    return true
  else 
    return false
  end
end

-- set the hurtData spawn
function setHurtData (id)
  if id == 0 then
    mq.cmd.vardata('hurtData2', 'null')
      --print('Spawn[0]')

    return
  end
  
  -- print('Spawn['..id..']')
  mq.cmd.vardata('hurtData2', 'Spawn['..id..']')
end





while true
do
  if mq.TLO.Group.Members() >=1 then
    for i = 0, mq.TLO.Group.Members() do
      hurtSpawn = mq.TLO.Group.Member(i)
      HURT.healpoint = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..hurtSpawn.Class.ShortName()).Value())
      if hurtSpawn.Dead() ~= 1 and hurtSpawn.Present() ~= 0 and rangeCheck(hurtSpawn.Distance(), 270) and healthCheck(hurtSpawn.PctHPs()) then
        HURT.swap = hurtSpawn.PctHPs()
        --print(HURT.swap .. ' ' .. HURT.pct)
        if HURT.swap <= HURT.pct then
          HURT.pct = HURT.swap
          HURT.id = hurtSpawn.ID()
          setHurtData(HURT.id)
        end
        --print('Name: ' .. hurtSpawn.DisplayName() .. ':' .. hurtSpawn.PctHPs() .. '% ['.. hurtSpawn.ID() ..']')
      end
      sleep(.1)
    end
    
  else
    setHurtData(hurtSpawn.ID())
    -- hurtSpawn = mq.TLO.Me
    --hurt['pct'] = hurtSpawn.PctHPs()
    -- if hurtSpawn.Dead() ~= 1 and healthCheck(hurtSpawn.PctHPs()) then
      -- hurt['pct'] = hurtSpawn.PctHPs()
      -- hurt['id'] = hurtSpawn.ID()
    --   setHurtData(hurtSpawn.ID())
    -- else 
    --   setHurtData(mq.TLO.Me.ID())
    -- end
    -- print(hurtSpawn.DisplayName() .. ':' .. hurtSpawn.PctHPs() .. '% [\at'.. hurtSpawn.ID() ..'\ax]')
    sleep(.5)
      
  end
  
end









--${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}*.PCTHO)
--mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..mq.TLO.Group.Member(i).Class.ShortName()).Value()



