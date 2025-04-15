#ifdef WORKBENCH

enum FixedSplineEndPoint {
	FIX_HEIGHEST_POINT, //Top to Bottm
	FIX_LOWEST_POINT, //Bottom to Top
}

[WorkbenchToolAttribute( name: "Correct Spline Slope", description: "Clamps the local slope on each spline point.", wbModules: {"WorldEditor"}, awesomeFontCode:0xf715)]
class DAB_CorrectSelectedSplineSlopeTool: WorldEditorTool
{
	[Attribute(defvalue: "1", desc: "Decides if the heighest or lowest point will stay fixed. The other one might be moved!", category: "Slope Correction", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(FixedSplineEndPoint))]
	protected FixedSplineEndPoint m_fixedPoint;
	
	[Attribute(defvalue: "false", desc: "Copy the old tangents to the moved points, otherwise they will be reset!", category: "Slope Correction", uiwidget: UIWidgets.CheckBox)]
	protected bool m_keepTangents;
	
	[Attribute(defvalue: "1", desc: "Minimal local slope", category: "Slope Correction")]
	protected float m_minSlope;
	
	[Attribute(defvalue: "70", desc: "Maximum local slope", category: "Slope Correction")]
	protected float m_maxSlope;
	
	//------------------------------------------------------------------------------------------------
	protected override void OnActivate()
	{
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnDeActivate()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Analyze Splines")]
	protected void AnalyzeSplines()
	{		
		array<IEntitySource> selectedSplines = GetSelectedSplines();
				
		for (int i; i < selectedSplines.Count(); i++)
		{
			IEntitySource splineSrc = selectedSplines[i];
			AnalyzeSpline(splineSrc);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AnalyzeSpline(IEntitySource splineSrc)
	{	
		SplineShapeEntity spline = SplineShapeEntity.Cast(m_API.SourceToEntity(splineSrc));
		if(!spline) {
			Print("Something went wrong when converting a Entity to Spline. Skipping...", LogLevel.WARNING);
			return;
		}
		
		array<vector> positions = {};
		spline.GetPointsPositions(positions);
		if(positions.Count() < 3) {
			Print("To analyze a spline it needs at least three points. Skipping...", LogLevel.WARNING);
			return;
		}
		
		
		float lowestSlope = float.MAX;
		float highestSlope = -float.MAX;
		float averageCount = 0;
			
		
		//Ignore first point
		for (int j = positions.Count() - 2; j >= 0; j--){
			vector currentPoint = positions[j];
			vector previousPoint = positions[j+1];

			float realDistance= vector.Distance(previousPoint, currentPoint);
			float actualHeight = currentPoint[1] - previousPoint[1];
			float slope = Math.Asin(actualHeight / realDistance) * Math.RAD2DEG;
			
			lowestSlope = Math.Min(lowestSlope, Math.AbsFloat(slope));
			highestSlope = Math.Max(highestSlope, Math.AbsFloat(slope));
			averageCount += slope;
		}
		
		Print("Values for spline: " + spline.GetName() + " ------------");
		Print("Lowest Slope: " + lowestSlope);
		Print("Heighest Slope: " + highestSlope);
		Print("Average Slope: " + (averageCount / (positions.Count() - 1)));
	}
	
	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Correct Slopes")]
	protected void CorrectSlopes()
	{		
		array<IEntitySource> selectedSplines = GetSelectedSplines();
				
		for (int i; i < selectedSplines.Count(); i++)
		{
			IEntitySource splineSrc = selectedSplines[i];
			CorrectSpline(splineSrc);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected array<IEntitySource> GetSelectedSplines() {
		array<IEntitySource> selectedSplines = {};
		
		for (int i; i < m_API.GetSelectedEntitiesCount(); i++)
		{
			IEntitySource entSrc = m_API.GetSelectedEntity(i);
			
			if(!entSrc || !m_API.SourceToEntity(entSrc).IsInherited(SplineShapeEntity)) continue;
			
			//if(!DoesSplineContainRiver(entSrc)) continue;
			
			if(selectedSplines.Contains(entSrc)) continue;
			selectedSplines.Insert(entSrc);
		}
		
		return selectedSplines;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool DoesSplineContainRiver(IEntitySource splineSrc){
		for(int i; i < splineSrc.GetNumChildren(); i++){
			IEntitySource childSrc = splineSrc.GetChild(i);
			
			if(childSrc && m_API.SourceToEntity(childSrc).IsInherited(RiverEntity)) return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CorrectSpline(IEntitySource splineSrc){
		SplineShapeEntity spline = SplineShapeEntity.Cast(m_API.SourceToEntity(splineSrc));
		if(!spline) {
			Print("Something went wrong when converting a Entity to Spline. Skipping...", LogLevel.WARNING);
			return;
		}
		
		bool didChangeHeights = false;
		array<vector> positions = {};
		spline.GetPointsPositions(positions);
		if(positions.Count() < 3) {
			Print("To correct a spline it needs at least three points. Skipping...", LogLevel.WARNING);
			return;
		}
		
		float startHeight = positions[0][1];
		float endHeight = positions[positions.Count() - 1][1];
		
		Print("startHeight: " + startHeight + "; endHeight: " + endHeight);
		
		int adjustedPointCount = 0;
		bool goingUphill = m_fixedPoint == FixedSplineEndPoint.FIX_LOWEST_POINT;
		bool countFromBackToFront = (goingUphill && (endHeight < startHeight)) 
									|| (!goingUphill && (endHeight > startHeight));
		
		if(countFromBackToFront){
			
			Print("Back -> Front");
			//Ignore first point
			for (int j = positions.Count() - 2; j >= 0; j--){
				vector currentPoint = positions[j];
				vector previousPoint = positions[j+1];

				float distanceXZ = vector.DistanceXZ(previousPoint, currentPoint);
				
				float actualHeight = currentPoint[1] - previousPoint[1];
				if(!goingUphill) actualHeight = previousPoint[1] - currentPoint[1];	
				
				float minHeight = Math.Tan(Math.DEG2RAD * m_minSlope) * distanceXZ;
				float maxHeight = Math.Tan(Math.DEG2RAD * m_maxSlope) * distanceXZ;
				
				if(actualHeight > minHeight && actualHeight < maxHeight) continue;
				
				didChangeHeights = true;
				adjustedPointCount++;
				
				actualHeight = Math.Clamp(actualHeight, minHeight, maxHeight);
				if(goingUphill){
					currentPoint[1] = previousPoint[1] + actualHeight;
				} else {
					currentPoint[1] = previousPoint[1] - actualHeight;
				}
				

				positions[j] = currentPoint;
			}
		} else {
			
			Print("Front -> Back");
			//Ignore first point
			for (int j = 1; j < positions.Count(); j++){
				vector previousPoint = positions[j-1];
				vector currentPoint = positions[j];
				
				float distanceXZ = vector.DistanceXZ(previousPoint, currentPoint);
				
				float actualHeight = previousPoint[1] - currentPoint[1];
				if(goingUphill) actualHeight = currentPoint[1] - previousPoint[1];
				
				float minHeight = Math.Tan(Math.DEG2RAD * m_minSlope) * distanceXZ;
				float maxHeight = Math.Tan(Math.DEG2RAD * m_maxSlope) * distanceXZ;
				
				if(actualHeight > minHeight && actualHeight < maxHeight) continue;
				
				didChangeHeights = true;
				adjustedPointCount++;
				
				actualHeight = Math.Clamp(actualHeight, minHeight, maxHeight);
				currentPoint[1] = previousPoint[1] - actualHeight;
				
				if(!goingUphill){
					currentPoint[1] = previousPoint[1] - actualHeight;
				} else {
					currentPoint[1] = previousPoint[1] + actualHeight;
				}
			
				positions[j] = currentPoint;
			}
		}
		
		if(didChangeHeights) {
			array<vector> tangents = {};
			
			for (int i = 0; i < positions.Count(); i++){
				vector inTan, outTan;
				spline.GetTangents(i, inTan, outTan);
				tangents.Insert(inTan);
				tangents.Insert(outTan);
			}
			

			m_API.BeginEntityAction("CorrectingSplineElevation", "");
			
			DAB_ShapeHelper.ModifyPolyline(splineSrc, positions);
			
			if(m_keepTangents)
				CopyTangents(splineSrc, tangents);
			
			DAB_ShapeHelper.FixTerrainAdjustmentGenerators(splineSrc);
			
			m_API.EndEntityAction("CorrectingSplineElevation");
		}
		
		Print("Adjusted " + adjustedPointCount + "/" + positions.Count() + " points of the Spline.");
	}
	
	protected void CopyTangents(IEntitySource splineSrc, array<vector> tangents)
	{
		BaseContainerList points = splineSrc.GetObjectArray("Points");
		
		for (int j = 0; j < points.Count(); j++){
			BaseContainer point = points[j];
			BaseContainerList pointData = point.GetObjectArray("Data");
			
			if (pointData.Count() == 0)
			{
				m_API.CreateObjectArrayVariableMember(point, null, "Data", "SplinePointData", 0);
			}

			array<ref ContainerIdPathEntry> containerPath = {ContainerIdPathEntry("Points", j), ContainerIdPathEntry("Data", 0)};
			
			vector inTangent = tangents[j * 2];
			vector outTangent = tangents[(j * 2) + 1];
			
			m_API.SetVariableValue(splineSrc, containerPath, "InTangent", inTangent.ToString(false));	
			m_API.SetVariableValue(splineSrc, containerPath, "OutTangent", outTangent.ToString(false));		
		}
	}

}
#endif // WORKBENCH
