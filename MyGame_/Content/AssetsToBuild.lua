--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout.shader", arguments = { "vertex" } },
	},
	meshes = {
		"Meshes/cube.mesh",
		"Meshes/plane.mesh",
		"Meshes/circle.mesh",
		"Meshes/train.mesh",
	},
	effects = {
		"Effects/purple.effect",
		"Effects/white.effect",
		"Effects/red.effect",
		"Effects/blue.effect",
	},
}
