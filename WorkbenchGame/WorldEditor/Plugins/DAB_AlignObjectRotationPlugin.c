#ifdef WORKBENCH

[WorkbenchPluginAttribute( name: "Copy Rotation", description: "Copies rotation to selected objects", category: "DAB - Misc", shortcut: "Shift+Alt+W", wbModules: {"WorldEditor"}, awesomeFontCode: 0xF24D)]
class AlignObjectTransformsFromLastSelectedPlugin : WorkbenchPlugin
{
    override void Run()
    {
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		if (!worldEditor)
			return;

		WorldEditorAPI worldEditorAPI = worldEditor.GetApi();
		if (!worldEditorAPI)
			return;
		
		worldEditorAPI.BeginEntityAction();
		
		int entityCount = worldEditorAPI.GetSelectedEntitiesCount();
		if(entityCount <= 1){
			worldEditorAPI.EndEntityAction();
			return;
		}
			
		IEntitySource lastSelectedSource = worldEditorAPI.GetSelectedEntity(entityCount - 1);
		IEntity lastSelectedEntity = worldEditorAPI.SourceToEntity(lastSelectedSource);
		
		
		string angleY;
		lastSelectedSource.Get("angleY", angleY);
	
		for (int i, count = (entityCount - 1); i < count; i++)
		{
			IEntitySource entitySource = worldEditorAPI.GetSelectedEntity(i);
			
			worldEditorAPI.SetVariableValue(entitySource, {}, "angleY", angleY);
		}
		
		worldEditorAPI.SetEntitySelection( worldEditorAPI.GetSelectedEntity(0));
		
		worldEditorAPI.EndEntityAction();
	}
}

#endif // WORKBENCH
