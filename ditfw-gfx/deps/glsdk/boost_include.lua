newoption
{
   trigger     = "boost-dir",
   description = "Full path to the directory for the Boost distribution."
}

local internalBoostLoc = depsdir .. "/boost_1_55"

if(not BoostDir) then
	function BoostDir()
		return _OPTIONS["boost-dir"] or internalBoostLoc
	end
end