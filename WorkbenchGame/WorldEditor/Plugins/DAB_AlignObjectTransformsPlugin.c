#ifdef WORKBENCH

[WorkbenchPluginAttribute( name: "Copy Transform", category: "DAB - Misc", description: "Copies transform data to selected objects", shortcut: "Ctrl+Shift+W", wbModules: {"WorldEditor"}, awesomeFontCode: 0xF24D)]
class AlignObjectTransformsToLastSelectedPlugin : WorkbenchPlugin
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

		string coords, angleX, angleY, angleZ;
		lastSelectedSource.Get("coords", coords);
		lastSelectedSource.Get("angleX", angleX);
		lastSelectedSource.Get("angleY", angleY);
		lastSelectedSource.Get("angleZ", angleZ);
		
		string placementMode;
		EntityFlags lastFlags, newFlags;
		for (int i, count = (entityCount - 1); i < count; i++)
		{
			IEntitySource entitySource = worldEditorAPI.GetSelectedEntity(i);
			
			//Rotation
			worldEditorAPI.SetVariableValue(entitySource, {}, "angleX", angleX);
			worldEditorAPI.SetVariableValue(entitySource, {}, "angleY", angleY);
			worldEditorAPI.SetVariableValue(entitySource, {}, "angleZ", angleZ);

			//Placement
			lastSelectedSource.Get("placement", placementMode);
			worldEditorAPI.SetVariableValue(entitySource, {}, "placement", placementMode);
			
			//Flags - Relative_Y
			lastSelectedSource.Get("Flags", lastFlags);
			entitySource.Get("Flags", newFlags);
			
			if(lastFlags & EntityFlags.RELATIVE_Y) {
				newFlags |= EntityFlags.RELATIVE_Y;
			} else {
				newFlags &= ~EntityFlags.RELATIVE_Y;
			}
			
			worldEditorAPI.SetVariableValue(entitySource, {}, "Flags", (newFlags).ToString());
		
			//Position
			worldEditorAPI.SetVariableValue(entitySource, {}, "coords", coords);
		}
		
		worldEditorAPI.SetEntitySelection( worldEditorAPI.GetSelectedEntity(0));
		
		worldEditorAPI.EndEntityAction();
	}
}

#endif // WORKBENCH
