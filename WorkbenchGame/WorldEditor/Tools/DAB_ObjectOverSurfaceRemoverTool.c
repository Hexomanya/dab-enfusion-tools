#ifdef WORKBENCH
[WorkbenchToolAttribute(name: "Select Objects over Surface",description: "Selects objects over the defined Terrain Surfaces",wbModules: {"WorldEditor"}, awesomeFontCode: 0xF12D)]
class DAB_ObjectOverSurfaceRemover : WorldEditorTool
{
	[Attribute(defvalue: "", desc: "Blacklisted Surfaces")]
	protected ref array<ResourceName> m_blacklistedSurfaces;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.CheckBox, desc: "If enabled, also selects objects above roads", category: "Terrain")]
	private bool m_roadSelection;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.CheckBox, desc: "If enabled, also selects objects above rivers", category: "Terrain")]
	private bool m_riverSelection;
	
	protected BaseWorld m_World;
	
	protected string GetSurfaceMaterial(vector pos)
	{
		if(!m_World){
			Print("There is no world to trace!", LogLevel.ERROR);
			return "";
		}
		
		pos[1] = m_API.GetTerrainSurfaceY(pos[0], pos[2]);
		
		TraceParam params = new TraceParam();
		params.Flags = TraceFlags.WORLD;
		params.Start = pos + 0.01 * vector.Up;
		params.End = pos - 50 * vector.Up;
		
		m_World.TraceMove(params, null);
		
		return params.TraceMaterial;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void OnActivate()
	{
		m_World = m_API.GetWorld();
		if (!m_World) {
			Print("There is not world loaded!", LogLevel.ERROR);
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnDeActivate()
	{
		m_World = null;
	}

	//------------------------------------------------------------------------------------------------
	[ButtonAttribute("Select Entities")]
	protected void SelectEntities()
	{
		if(m_blacklistedSurfaces.Count() < 1){
			Print("You need to set a surface first!", LogLevel.WARNING);
			return;
		}
		
		int entityCount = m_API.GetSelectedEntitiesCount();
		
		m_API.BeginEntityAction();
		
		for (int i; i < entityCount; i++)
		{
			IEntitySource entitySource = m_API.GetSelectedEntity(i);
			
			vector coords;
			entitySource.Get("coords", coords);
			
			string currentSurface = GetSurfaceMaterial(coords);
			
			if(!IsOnBlacklistedSurfaces(currentSurface)){
				m_API.RemoveFromEntitySelection(entitySource);
			}
		}
		
		m_API.EndEntityAction();
	}
	
	protected bool IsOnBlacklistedSurfaces(string pathName){
		foreach(ResourceName blacklistSurface: m_blacklistedSurfaces)
		{
			if(blacklistSurface.GetPath() == pathName) return true;
		}
		
		return false;
	}
	
}
#endif
