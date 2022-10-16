--[[ 

testing group health return
  
--]]




require "common"




local range = {}
range['envrad'] = mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value()
range['myrange'] = mq.TLO.Spell('Guileless Remedy Rk. III').MyRange() 

local hurt = {}
hurt['pct'] = 101
hurt['id'] = 0





local healthCheck = function (memberPct)
  
  if memberPct <= hurt['pct'] then
    mq.cmd.echo('true')
    return true
  else 
    return false
  end
  
  -- return math.min(memberPct, PctCheck)
end

local rangeCheck = function (memberRange, range)
  mq.cmd.echo(memberRange, unpack(range))
  return memberRange < math.max(unpack(range))
end



-- set the hurtData spawn
local hurtData = function (spawn_id)
  mq.cmd.vardata('hurtData', 'Spawn['..spawn_id..']')
end







for i = 0, mq.TLO.Group.Members()
do
  local group_member = mq.TLO.Group.Member(i) 
  if group_member.Dead() ~= 1 and group_member.Present() ~= 0 and rangeCheck(group_member.Distance(), range) and healthCheck(group_member.PctHPs()) then
    
    hurt['pct'] = group_member.PctHPs()
    hurt['id'] = group_member.ID()
    hurtData(hurt['id'])
    mq.cmd.echo(group_member.ID().."::"..group_member.DisplayName().."::"..group_member.PctHPs())
  end
  
   v
   
end












