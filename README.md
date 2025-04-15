# [DAB] Enfusion Tools
Tools for the "Enfusion Workbench" for the the game "Arma Reforger"
---
# Plugins
### Copy Transforms (Ctrl+Shift+W | DAB - Misc)
Copies position, rotation, placment and relative Y set from the last selected object to all previous selected objects.
Normally used when you place down walls or sidewalks manually and use different versions with the same origin.

### Copy Rotation (Alt+Shift+W | DAB - Misc)
Same as "Copy Transforms" but only copies over the Y rotation. 
Normally used when you want to allign objects, like a fence gate to a house.

### Fix Road Material Sort Settings (DAB - Misc)
You should normally not use the "Mat Sort" settings in the road entity (See BI Wiki).
This Plugin copies the number of the "Mat Sort" to "Road Sort". Please be advised that the result are not the same.
You also need to use the Bias settings in the road material to sort your roads.

# Tools
### Slope Correction Tool (Slash Icon)
This tool has two function:
- Correct Slope: Takes in a min and max slope and clamps each ShapePoint to it. The fixed point determines which side (end/start) will stay put and which side is allowed to move. The most common use case for this tool is to make sure that a river does not flow uphill. 
This tool only works from the point positions and does not take tangents into account when calculating the slope. So when you have long tangents and a low min slope value you could still get a river to flow uphill, but it is good enough in most cases. 
This tool also works when a road or line-Terrain-Shape-Entity is one the same spline.
- Analyze Splines: This tool returns the heighest, lowest and average slope of a spline. As with the correct slope tool, this takes only point position into account and ignores tangents.

### Select Over Surface Tool (Eraser Icon)
This tool selects all objects that are over user defined surface materials, roads, rivers or lakes. The most common use case is the mass select trees that are on a rock surface and delete them or to select them and move them to another layer to inspect them more easily.

### Prefabs Randomizer Tool (Crossing Arrows Icon)
This tool works a little bit like the vanilla "Entity Replacer" Tool. But this tool allows for configs that can be swapped, setup faster and allow for more options. The workflow intended for this tool is to place the normal undecoreated prefabs first and then to randomly replace them with decoreated ones that have the same origin (Picnic Tables, Graves, etc.).
It can also be used to randomized repeating assets like sidewalks.
This tool also works better with Relative Y and slopecontact when compared to the normal "Entity Replacer" Tool.

# Fixes
### (Fixed) Polyline Area Tool (Cow Icon (Yes, really))
This is the fixed version of the vanilla Polyline Area Tool. It no longer crashes the workbench and also automatically updates any roads that are on the transformed shape. This will be removed as soon as it is patched in vanilla.

### (Fixed) WallGenerator
This mods (overrides) the vanilla WallGenerator so it works with polylines again. This will be removed as soon as it is patched in vanilla.

---
# Known Issues
- Slope Correction Tool: Can throw error " Degenerated triangles detected" while correcting the slope. This issue can be ignored, because it fixes itself after doing the operation.

---
# License
This project is provided for free use and must always remain open source.

✅ You may download and use this project for personal or commercial purposes.

❌ You may not distribute modified versions of this project.

✅ You may share this project only in its original, unmodified form.

✅ You must include clear attribution (e.g., credit on the workshop page).

By using this project, you agree to the terms above.