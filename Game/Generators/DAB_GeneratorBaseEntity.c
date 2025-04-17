#ifdef WORKBENCH
modded class SCR_PowerlineGeneratorEntity
{
	override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		Print("We are in");
		super._WB_OnKeyChanged(src, key, ownerContainers, parent);

		if (m_bIsChangingWorkbenchKey)
			return false;

		WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
		if (!worldEditorAPI || worldEditorAPI.UndoOrRedoIsRestoring())
			return true;

		m_ParentShapeSource = GetShapeSource();
		bool sameParentChange = parent && worldEditorAPI.EntityToSource(parent) == m_ParentShapeSource;

		// keep the scale at 1
		if (key == "scale")
		{
			bool isSetDirectly = src.IsVariableSetDirectly("scale");

			if (sameParentChange)
				Print("Do not modify a generator entity itself! Change its parent shape instead", LogLevel.WARNING);

			m_bIsChangingWorkbenchKey = true;
			if (isSetDirectly)
			{
				worldEditorAPI.ClearVariableValue(src, null, "scale");
			}
			else
			{
				worldEditorAPI.SetVariableValue(src, null, "scale", "1");
				Print("Generator has scale set in Prefab! " + src.GetResourceName(), LogLevel.WARNING);
			}

			m_bIsChangingWorkbenchKey = false;
		}

		// keep the angles at 0
		array<string> angles = { "angleX", "angleY", "angleZ" };
		foreach (string angle : angles)
		{
			if (key == angle)
			{
				bool isSetDirectly = src.IsVariableSetDirectly(angle);

				if (sameParentChange)
					Print("Do not modify a generator entity itself! Change its parent shape instead", LogLevel.WARNING);

				m_bIsChangingWorkbenchKey = true;
				if (isSetDirectly)
				{
					worldEditorAPI.ClearVariableValue(src, null, angle);
				}
				else // angleValue != 0
				{
					worldEditorAPI.SetVariableValue(src, null, angle, "0");
					Print("Generator has angle " + angle + " set in Prefab! " + src.GetResourceName(), LogLevel.WARNING);
				}

				m_bIsChangingWorkbenchKey = false;
			}
		}

		// keep the generator at (relative) 0 0 0 as long as it has a parent
		// if no parent, do not set to 0 0 0
		// do not warn about no parent here as the constructor does it
		if (parent && key == "coords")
		{
			vector coords;
			src.Get("coords", coords);
			bool isSetDirectly = src.IsVariableSetDirectly("coords");
			if (isSetDirectly || coords != vector.Zero) // because this can trigger when changing parent shape's points
			{
				if (sameParentChange)
					Print("Do not modify a generator entity itself! Change its parent shape instead", LogLevel.WARNING);

				m_bIsChangingWorkbenchKey = true;
				if (isSetDirectly)
				{
					worldEditorAPI.ClearVariableValue(src, null, "coords");
				}
				else
				{
					worldEditorAPI.SetVariableValue(src, null, "coords", "0 0 0");
					Print("Generator has coords set in Prefab! " + src.GetResourceName(), LogLevel.WARNING);
				}

				m_bIsChangingWorkbenchKey = false;
			}
		}

		ShapeEntity parentShape = ShapeEntity.Cast(parent);
		if (parentShape)
			m_ParentShapeSource = worldEditorAPI.EntityToSource(parentShape);
		else
			m_ParentShapeSource = null;

		// let's not save here for now
		// BaseContainerTools.WriteToInstance(this, worldEditorAPI.EntityToSource(this));

		return true;
	}
}

#endif