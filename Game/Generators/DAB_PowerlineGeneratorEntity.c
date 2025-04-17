#ifdef WORKBENCH
/*modded class SCR_PowerlineGeneratorEntity
{
	protected override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the junction holder present on the provided point
	//! \param[in] point the relative position to check
	//! \param[out] sameLine whether the found junction is one of the current generator's junction points
	//! \return true on success, false on failure
	protected override IEntitySource FindJunctionSourceOnPointXZ(vector point, out bool sameLine)
	{
		WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
		if (!worldEditorAPI)
			return null;

		IEntity entity;

		foreach (IEntitySource junctionObject, IEntitySource junctionPole : m_mJunctionEntitySources)
		{
			entity = worldEditorAPI.SourceToEntity(junctionObject);
			if (!entity)
			{
				Print("[SCR_PowerlineGeneratorEntity.FindJunctionSourceOnPointXZ] null entity (own junction) (" + __FILE__ + " L" + __LINE__ + ")", LogLevel.WARNING);
				continue;
			}

			if (vector.DistanceSqXZ(point, CoordToLocal(entity.GetOrigin())) < TOLERANCE_SQUARED)
			{
				sameLine = true;
				return junctionObject;
			}
		}

		foreach (IEntitySource junctionObject, IEntitySource junctionPole : m_mOtherJunctionEntitySources)
		{
			if(!junctionObject || !junctionPole)
			{
				//Why does this even get picked up
				Print("junctionObject was: " + junctionObject);
				Print("junctionPole was: " + junctionPole);
				continue;
			}
			
			entity = worldEditorAPI.SourceToEntity(junctionObject); //This crashes
			if (!entity)
			{
				Print("[SCR_PowerlineGeneratorEntity.FindJunctionSourceOnPointXZ] null entity (others' junction) (" + __FILE__ + " L" + __LINE__ + ")", LogLevel.WARNING);
				continue;
			}

			if (vector.DistanceSqXZ(point, CoordToLocal(entity.GetOrigin())) < TOLERANCE_SQUARED)
			{
				sameLine = false;
				return junctionObject;
			}
		}

		return null;
	}
}*/

#endif