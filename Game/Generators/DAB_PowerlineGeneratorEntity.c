modded class SCR_PowerlineGeneratorEntity
{
	
	protected override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		//return true;
		/* The code from here is just to show, that it all works when you just set the return value to false */
		
		//SUPER.SUPER IS NOT SAFE (NOT SURE IT EVEN WORKS), TRYING TO AVOID VANILLA POWERLINE GENERATOR WITHOUT LOOSING CALL TO PARENT
		super.super._WB_OnKeyChanged(src, key, ownerContainers, parent);

		if (key == "coords")
			return false;
		
		Print("key: " + key);
		
		if (key == "Flags")
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

		return true; // WAS TRUE BEFORE, THIS CAUSES THE FACE NORTH BUG!
	}
	
	//------------------------------------------------------------------------------------------------
	//! Creates power lines between two power poles based on their cable types, if they have compatible slots
	//! \param[in] existingPole existing pole represents the current power pole in the scene
	//! \param[in] addedPole added pole represents the new pole being added to the power line
	//! \param[in] isSameLine true if poles are on the same line, false if a junction is connecting another line
	//! \return the created power line entity sources, or null array on error / no cables
	// TODO: fix cable bug on rotated shape
	protected override array<IEntitySource> CreatePowerLines(SCR_PowerPole existingPole, SCR_PowerPole addedPole, bool isSameLine)
	{
		WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
		if (!worldEditorAPI)
			return null;
		
		string existingAngle;
		worldEditorAPI.EntityToSource(existingPole).Get("angleY", existingAngle);
		
		Print("existingAngle source: " + existingAngle);
		Print("existingAngle entity: " + existingPole.GetAngles());
		Print("existingAngle YawPitchRoll: " + existingPole.GetYawPitchRoll());
		
		/*Print("addedAngle source: " + addedAngle);
		Print("addedAngle entity: " + addedPole.GetAngles());*/
		
		if (!existingPole || !addedPole || existingPole == addedPole)
			return null;

		vector existingPoleWorldPos = existingPole.GetOrigin();
		vector addedPoleWorldPos = addedPole.GetOrigin();

		map<SCR_EPoleCableType, ref SCR_PoleCableSlotGroup> existingPoleGroups = existingPole.GetClosestCableSlotGroupsForEachCableType(addedPoleWorldPos, isSameLine);
		if (existingPoleGroups.IsEmpty())
		{
			Print("No valid slots found on the existing power pole, please check the setup of your power poles.", LogLevel.WARNING);

			if (m_DebugShapeManager)
				m_DebugShapeManager.AddSphere(existingPoleWorldPos + DEBUG_POLE_POS, DEBUG_POLE_SIZE, DEBUG_POLE_COLOUR_ERROR, ShapeFlags.NOOUTLINE);

			return null;
		}

		map<SCR_EPoleCableType, ref SCR_PoleCableSlotGroup> addedPoleGroups = addedPole.GetClosestCableSlotGroupsForEachCableType(existingPoleWorldPos, isSameLine);
		if (addedPoleGroups.IsEmpty())
		{
			Print("No valid slots found on the new power pole, please check the setup of your power poles.", LogLevel.WARNING);

			if (m_DebugShapeManager)
				m_DebugShapeManager.AddSphere(addedPoleWorldPos + DEBUG_POLE_POS, DEBUG_POLE_SIZE, DEBUG_POLE_COLOUR_ERROR, ShapeFlags.NOOUTLINE);

			return null;
		}

		array<SCR_EPoleCableType> cableTypes = {};
		if (SCR_Enum.GetEnumValues(SCR_EPoleCableType, cableTypes) < 1) // like, wat
			return null;

		array<SCR_EPoleCableType> cableTypesInCommon = {};
		foreach (SCR_EPoleCableType cableType : cableTypes)
		{
			if (existingPoleGroups.Contains(cableType) && addedPoleGroups.Contains(cableType))
				cableTypesInCommon.Insert(cableType);

			// code below could be done here directly, but for clarity's sake, one foreach won't kill perfs
		}

		if (cableTypesInCommon.IsEmpty())
		{
			Print("No cable types in common between two poles - be sure to select compatible poles in Generator Prefab (" + existingPoleWorldPos.ToString(false) + ")", LogLevel.WARNING);

			if (m_DebugShapeManager)
			{
				m_DebugShapeManager.AddSphere(existingPoleWorldPos + DEBUG_POLE_POS, DEBUG_POLE_SIZE, DEBUG_POLE_COLOUR_ERROR, ShapeFlags.NOOUTLINE);
				m_DebugShapeManager.AddSphere(addedPoleWorldPos + DEBUG_POLE_POS, DEBUG_POLE_SIZE, DEBUG_POLE_COLOUR_ERROR, ShapeFlags.NOOUTLINE);
				m_DebugShapeManager.AddLine(existingPoleWorldPos + DEBUG_POLE_POS, addedPoleWorldPos + DEBUG_POLE_POS, DEBUG_POLE_COLOUR_ERROR);
			}

			return null;
		}

		array<IEntitySource> result = {};

		array<vector> startPoints = {};		// world position
		array<vector> endPoints = {};		// world position
		SCR_PoleCableSlotGroup existingGroup;
		SCR_PoleCableSlotGroup addedGroup;
		//WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
		IEntitySource powerLineEntitySource;
		IEntity referenceEntity;
		array<ref ContainerIdPathEntry> containerPath;
		foreach (SCR_EPoleCableType cableType : cableTypesInCommon)
		{
			existingGroup = existingPoleGroups.Get(cableType);
			int existingPoleCableCount = existingGroup.m_aSlots.Count();	// cannot be zero

			addedGroup = addedPoleGroups.Get(cableType);
			int addedPoleCableCount = addedGroup.m_aSlots.Count();			// cannot be zero

			int cableCount;
			if (existingPoleCableCount < addedPoleCableCount)
				cableCount = existingPoleCableCount;
			else
				cableCount = addedPoleCableCount;

			startPoints.Clear();
			endPoints.Clear();
			for (int i; i < cableCount; ++i)
			{
				startPoints.Insert(existingPole.CoordToParent(existingGroup.m_aSlots[i].m_vPosition));
				endPoints.Insert(addedPole.CoordToParent(addedGroup.m_aSlots[i].m_vPosition));
			}

			if (m_DebugShapeManager) // warn visually about unused slots
			{
				for (int i = cableCount; i < existingPoleCableCount; ++i)
				{
					m_DebugShapeManager.AddSphere(existingPole.CoordToParent(existingGroup.m_aSlots[i].m_vPosition), DEBUG_SLOT_SIZE, DEBUG_SLOT_COLOUR_UNUSED, ShapeFlags.NOOUTLINE);
				}

				for (int i = cableCount; i < addedPoleCableCount; ++i)
				{
					m_DebugShapeManager.AddSphere(addedPole.CoordToParent(addedGroup.m_aSlots[i].m_vPosition), DEBUG_SLOT_SIZE, DEBUG_SLOT_COLOUR_UNUSED, ShapeFlags.NOOUTLINE);
				}
			}

			float normalLengthSq;
			float reversedLengthSq;
			foreach (int i, vector startPoint : startPoints)
			{
				normalLengthSq += vector.DistanceSq(startPoint, endPoints[i]);
				reversedLengthSq += vector.DistanceSq(startPoint, endPoints[cableCount - i - 1]);
			}

			// don't cross the streams!
			if (reversedLengthSq < normalLengthSq)
				SCR_ArrayHelperT<vector>.Reverse(endPoints);

			if (m_DebugShapeManager)
			{
				foreach (int i, vector startPoint : startPoints)
				{
					m_DebugShapeManager.AddSphere(startPoint, DEBUG_SLOT_SIZE, DEBUG_SLOT_COLOUR_POS, ShapeFlags.NOOUTLINE);
					m_DebugShapeManager.AddSphere(endPoints[i], DEBUG_SLOT_SIZE, DEBUG_SLOT_COLOUR_POS, ShapeFlags.NOOUTLINE);
					m_DebugShapeManager.AddLine(startPoint, endPoints[i], DEBUG_CABLE_COLOUR_POS);
				}
			}

			// power line preparation
			powerLineEntitySource = null;

			string powerLineEntity;
			if (m_sPowerLinePrefab) // !.IsEmpty()
				powerLineEntity = m_sPowerLinePrefab;
			else
				powerLineEntity = POWER_LINE_CLASS;

			vector startPos, endPos;
			// Create cables for the entity

			int cableIndex;
			foreach (int i, vector startPoint : startPoints)
			{
				if (powerLineEntitySource)
				{
					referenceEntity = worldEditorAPI.SourceToEntity(powerLineEntitySource);
					startPos = referenceEntity.CoordToLocal(startPoint);
					endPos = referenceEntity.CoordToLocal(endPoints[i]);
				}
				else
				{
					referenceEntity = worldEditorAPI.SourceToEntity(m_Source);
					startPos = referenceEntity.CoordToLocal(startPoint);
					endPos = referenceEntity.CoordToLocal(endPoints[i]);
				}

				if (vector.DistanceSqXZ(startPos, endPos) < MIN_CABLE_LENGTH_SQ)
				{
					Print("Cable's 2D length is too small! Skipping (" + vector.DistanceXZ(startPos, endPos).ToString(-1, 2) + "m < " + Math.Sqrt(MIN_CABLE_LENGTH_SQ) + "m)", LogLevel.WARNING);

					if (m_DebugShapeManager)
					{
						float localX = existingPole.CoordToLocal(startPoint)[0];
						vector forward = vector.Direction(startPoint, endPoints[i]); // not normalised
						vector right = -{
							forward[0] * Math.Cos(Math.PI_HALF) - forward[2] * Math.Sin(Math.PI_HALF),
							0,													// Y rotation - 2D only
							forward[0] * Math.Sin(Math.PI_HALF) + forward[2] * Math.Cos(Math.PI_HALF),
						};

						if (localX > DEBUG_CABLE_LENGTH_POS_MAX_DIST)
							right *= DEBUG_CABLE_LENGTH_POS_MAX_DIST;
						else
							right *= localX;

						vector spherePos = (startPoint + endPoints[i]) * 0.5 + DEBUG_CABLE_LENGTH_POS + right;

						m_DebugShapeManager.AddSphere(spherePos, DEBUG_CABLE_LENGTH_SIZE, DEBUG_CABLE_LENGTH_COLOUR_ERROR_SPHERE, ShapeFlags.NOOUTLINE);
						m_DebugShapeManager.AddLine(spherePos, startPoint, DEBUG_CABLE_LENGTH_COLOUR_ERROR_LINE);
						m_DebugShapeManager.AddLine(spherePos, endPoints[i], DEBUG_CABLE_LENGTH_COLOUR_ERROR_LINE);
					}

					continue;
				}

				if (!powerLineEntitySource)
				{
					powerLineEntitySource = worldEditorAPI.CreateEntity(powerLineEntity, string.Empty, m_iSourceLayerID, m_Source, referenceEntity.CoordToLocal(existingPoleWorldPos), vector.Zero);
					if (!powerLineEntitySource)
					{
						Print("CreateEntity returned null trying to create " + powerLineEntity, LogLevel.ERROR);
						return null; // it would fail for the other ones too
					}

					// get the most adapted material
					ResourceName cableMaterial;
					foreach (SCR_PoleCable cable : m_aCables)
					{
						if (cable.m_eType == cableType && cable.m_sMaterial) // !.IsEmpty()
						{
							cableMaterial = cable.m_sMaterial;
							break;
						}
					}

					if (!cableMaterial)
						cableMaterial = m_PowerlineMaterial; // default value

					if (cableMaterial)
						worldEditorAPI.SetVariableValue(powerLineEntitySource, null, "Material", cableMaterial);

					referenceEntity = worldEditorAPI.SourceToEntity(powerLineEntitySource);
					startPos = referenceEntity.CoordToLocal(startPoint);
					endPos = referenceEntity.CoordToLocal(endPoints[i]);
				}

				worldEditorAPI.CreateObjectArrayVariableMember(powerLineEntitySource, null, "Cables", "Cable", cableIndex);
				containerPath = { new ContainerIdPathEntry("Cables", cableIndex) };
				worldEditorAPI.SetVariableValue(powerLineEntitySource, containerPath, "StartPoint", startPos.ToString(false));
				worldEditorAPI.SetVariableValue(powerLineEntitySource, containerPath, "EndPoint", endPos.ToString(false));
				cableIndex++;
			}

			if (powerLineEntitySource)
				result.Insert(powerLineEntitySource);
		}

		return result;
	}
}