#ifdef WORKBENCH


//WARNING THIS IS VERY SLOW BECAUSE WE HAVE TO UNPARENT EACH ENTITY ONE BY ONE
//A BETTER WORKFLOW IS TO USE: SELECT CHILDREN -> REMOVE FROM PARENT -> CHANGE ABSOLUTE TO Y TO RELATIVE
[WorkbenchPluginAttribute( name: "Extract Forests", description: "Removes generated Entities from selected Generator-Parents", wbModules: {"WorldEditor"}, awesomeFontCode: 0xF1BB)]
class ExtractObjectsFromForestGeneratorPlugin : WorkbenchPlugin
{
    override void Run()
    {
		
		Print("WARNING THIS IS VERY SLOW BECAUSE WE HAVE TO UNPARENT EACH ENTITY ONE BY ONE!", LogLevel.WARNING);
		Print("A BETTER WORKFLOW IS TO USE: SELECT CHILDREN -> REMOVE FROM PARENT -> CHANGE ABSOLUTE TO Y TO RELATIVE", LogLevel.WARNING);
		/*WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		if (!worldEditor)
			return;

		WorldEditorAPI worldEditorAPI = worldEditor.GetApi();
		if (!worldEditorAPI)
			return;
		
		int selectedCount = worldEditorAPI.GetSelectedEntitiesCount();
		if (selectedCount == 0)
		{
			Print("You need to select at least one entity.", LogLevel.ERROR);
			return;
		}

		worldEditorAPI.BeginEntityAction("Extracting all Forests");
		
		for (int i = 0; i < selectedCount; i++)
		{
			IEntitySource src = worldEditorAPI.GetSelectedEntity(i);

			if(! src.GetClassName().ToType().IsInherited(((typename)PolylineShapeEntity))) continue;
			vector ancestorPos;
			src.Get("coords", ancestorPos);
			
			for(int j = 0; j < src.GetNumChildren(); j++){
				IEntitySource child = src.GetChild(j);
				
				if(! child.GetClassName().ToType().IsInherited(((typename)GeneratorBaseEntity))) continue;
				WBProgressDialog progress = new WBProgressDialog("Extracting Objects..", worldEditor);
				
				vector parentPos;
				child.Get("coords", parentPos);
				
				int forestObjectCount = child.GetNumChildren();
				
				array<IEntitySource> forestObjects = {};
				
				for(int x = 0; x < forestObjectCount; x++){	
					IEntitySource forestObject = child.GetChild(x);
					if(!forestObject) continue;
					forestObjects.Insert(forestObject);
				}
				
				for(int x = 0; x < forestObjects.Count(); x++){
					IEntitySource removeObject = forestObjects.Get(x);
					
					vector localPos;
					removeObject.Get("coords", localPos);
					
					worldEditorAPI.RemoveEntityFromParent(removeObject);
					
					EntityFlags flags; 
					removeObject.Get("Flags", flags);
					localPos = ancestorPos + parentPos + localPos;
					
					if(flags & EntityFlags.RELATIVE_Y) {
						float y;
						if(worldEditorAPI.TryGetTerrainSurfaceY(localPos[0],localPos[2],y)) localPos[1] = localPos[1] - y;
					}
					
					worldEditorAPI.SetVariableValue(removeObject, null, "coords", localPos.ToString(false));

					progress.SetProgress(x / forestObjects.Count());
				}
			}
		}
		
		worldEditorAPI.EndEntityAction();*/
	}
}

#endif // WORKBENCH
