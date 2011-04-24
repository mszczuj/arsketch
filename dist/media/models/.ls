material material_matte
{
	receive_shadows on
	technique
	{
		pass
		{
			ambient 0.500000 0.500000 0.500000 0.000000
			diffuse 0.640000 0.640000 0.640000 0.000000
			specular 0.000000 0.000000 0.000000 0.000000 0.250000
			emissive 0.000000 0.000000 0.000000 0.000000
			scene_blend alpha_blend
			depth_write off
		}
	}
}
