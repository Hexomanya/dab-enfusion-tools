#ifdef WORKBENCH
[WorkbenchToolAttribute("(Fixed) Polyline Area Tool", "Turns polylines into polyline areas, select a polyline, then click 'Convert'", awesomeFontCode: 0xF6C8)]
class DAB_PolylineAreaTool : WorldEditorTool
{
	//------------------ Rectangle Settings ----------------------------------------------------------
	[Attribute(defvalue: "5", category: "Rectangle Settings")]
	protected float m_fRectangleWidth;

	[Attribute(defvalue: "3", category: "Rectangle Settings")]
	protected float m_fRectangleHeight;

	//------------------ Circle Materials ------------------------------------------------------------
	[Attribute(defvalue: "3", category: "Circle Settings")]
	protected float m_fCircleRadius;

	[Attribute(defvalue: "16", category: "Circle Settings")]
	protected float m_fCircleSegments;

	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Turn to Rect")]
	protected void MakeRec()
	{
		//TODO: do once somewhere ?
		IEntitySource entSrc = m_API.GetSelectedEntity();
		if (!entSrc || !m_API.SourceToEntity(entSrc).IsInherited(ShapeEntity))
			return;

		vector lowerLeft, lowerRight, upperLeft, upperRight;

		lowerLeft[0] = 0 - m_fRectangleWidth * 0.5;
		lowerLeft[2] = 0 - m_fRectangleHeight * 0.5;

		lowerRight[0] = m_fRectangleWidth * 0.5;
		lowerRight[2] = 0 - m_fRectangleHeight * 0.5;

		upperLeft[0] = 0 - m_fRectangleWidth * 0.5;
		upperLeft[2] = m_fRectangleHeight * 0.5;

		upperRight[0] = m_fRectangleWidth * 0.5;
		upperRight[2] = m_fRectangleHeight * 0.5;

		array<vector> positions = {};
		positions.Insert(lowerLeft);
		positions.Insert(lowerRight);
		positions.Insert(upperRight);
		positions.Insert(upperLeft);

		m_API.BeginEntityAction("", "");
		DAB_ShapeHelper.ModifyPolyline(entSrc, positions);
		DAB_ShapeHelper.FixTerrainAdjustmentGenerators(entSrc);
		m_API.EndEntityAction("");
	}

	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Turn to Circle")]
	protected void MakeCircle()
	{
		//TODO: do once somewhere ?
		IEntitySource entSrc = m_API.GetSelectedEntity();
		if (!entSrc || !m_API.SourceToEntity(entSrc).IsInherited(ShapeEntity))
			return;

		array<vector> positions = {};
		float segmentSize = Math.PI2 / m_fCircleSegments;

		for (float i = 0; i < Math.PI2; )
		{
			float x = Math.Cos(i) * m_fCircleRadius;
			float y = Math.Sin(i) * m_fCircleRadius;
			i += segmentSize;

			positions.Insert({ x, 0, y });
		}

		m_API.BeginEntityAction("", "");
		DAB_ShapeHelper.ModifyPolyline(entSrc, positions);
		DAB_ShapeHelper.FixTerrainAdjustmentGenerators(entSrc);
		m_API.EndEntityAction("");
	}

	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Convert")]
	protected void Convert()
	{
		IEntitySource src = m_API.GetSelectedEntity();
		if (!src)
			return;

		IEntity entity = m_API.SourceToEntity(src);
		if (!entity.IsInherited(ShapeEntity))
			return;
		
		m_API.BeginEntityAction("", "");

		IEntitySource parentSrc = src.GetParent();
		if (!parentSrc)
		{
			parentSrc = m_API.CreateEntity("PolylineArea","", src.GetLayerID(), null, entity.GetOrigin(), "0 0 0");
			m_API.ParentEntity(parentSrc, src, true);
		}

		bool foundComp;
		for (int i = 0, count = src.GetComponentCount(); i < count; i++)//workaround for nonfunctional FindComponent(Hierarchy)
		{
			IEntityComponentSource comp = src.GetComponent(i);

			if (comp.GetClassName() == "Hierarchy")
			{
				foundComp = true;
				break;
			}
		}

		if (!foundComp)
			m_API.CreateComponent(src, "Hierarchy");

		m_API.EndEntityAction("");

/*
		proto native IEntityComponentSource GetComponentAt(IEntitySource owner, int index);

		if (!entity.FindComponent(HierarchyComponent))
		{
			m_API.CreateComponent(src, "Hierarchy");
			m_API.EndEntityAction("");
		}
*/
	}
}
#endif // WORKBENCH
