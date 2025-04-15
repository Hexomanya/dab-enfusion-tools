modded class SCR_PowerlineGeneratorEntity
{
	private bool m_shapeChangeLock = false;
	
	protected override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		Print("Key change!");
		
		super._WB_OnKeyChanged(src, key, ownerContainers, parent);

		if (key == "coords")
			return false;

		WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
		if (!worldEditorAPI || worldEditorAPI.UndoOrRedoIsRestoring())
			return false;

		BaseContainerTools.WriteToInstance(this, worldEditorAPI.EntityToSource(this));
		if (m_ParentShapeSource)
			OnShapeInit(m_ParentShapeSource, ShapeEntity.Cast(worldEditorAPI.SourceToEntity(m_ParentShapeSource)));
		// TODO instead of OnShapeInit
//		if (m_ParentShapeSource)
//			GenerateGeneratorJunctions(src, m_bDrawDebugShapes);

		return true;
	}
	
	protected override void OnShapeChangedInternal(IEntitySource shapeEntitySrc, ShapeEntity shapeEntity, array<vector> mins, array<vector> maxes)
	{
		if(m_shapeChangeLock)
		{
			Print("OnShapeChangedInternal was not executed, because it would override OnPointChangedInternal");
			m_shapeChangeLock = false;
			return;
		}
		
		super.OnShapeChangedInternal(shapeEntitySrc, shapeEntity, mins, maxes);

		if (!m_bEnableGeneration)
		{
			Print("Power line generation is disabled for this shape (beware of junction issues with other lines) - tick it back on before saving", LogLevel.NORMAL);
			return;
		}

		m_RandomGenerator.SetSeed(m_iSeed);

		WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
		if (!worldEditorAPI || worldEditorAPI.UndoOrRedoIsRestoring())
			return;

		bool manageEntityAction = !worldEditorAPI.IsDoingEditAction();
		
		if (manageEntityAction)
			worldEditorAPI.BeginEntityAction();

		GenerateGeneratorJunctions(m_Source, m_bDrawDebugShapes); // TODO handle this case better if needed, use the bbox arrays

		if (manageEntityAction)
			worldEditorAPI.EndEntityAction();
	}
	
	protected override void OnPointChangedInternal(IEntitySource shapeEntitySrc, ShapeEntity shapeEntity, PointChangedSituation situation, int pointIndex, vector position)
	{
		m_shapeChangeLock = true;
		
		super.OnPointChangedInternal(shapeEntitySrc, shapeEntity, situation, pointIndex, position);
		
		/*
			- Is changed point junction or end/start point?
			--> Otherwise we have to delete every pole between index - 1 to index + 1
		*/
		
		//Polecount = entire distance / maxdistance
	}
}