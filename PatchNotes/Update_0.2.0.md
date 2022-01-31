Update 5 support, additional helpful warnings, many bugfixes. New docs hosted on ficsit.app. **Important: New script folder locations!** This update brought to you by Robb#6731 and Irwon




## Summary

Thank you for being patient while I (Robb) updated this mod. I know relatively little about the magic Nog has done to make it all work behind the scenes, and the code is not the cleanest. As a result, changes to this mod take quite a while for me to make. Please be patient in case of any bugs or oddities.

Also, it could take a bit for other mod devs to update their mods (that depend on ContentLib), so please give them some time as well.

**WIP updated documentation can be found [here](https://docs.ficsit.app/contentlib/latest/index.html) instead of the mod page,** since it's easier to keep up to date and allow others to edit.
I don't have the time to fully update them now, but feel free to contribute via Pull Requests or suggestions on the Discord.

For people who have used ContentLib before, these patch notes should do a pretty good job of covering all that changed. Head to the [Nog's Mods discord](https://discord.gg/kcRmFxn89d) for support.

Big thanks to Irwon on the Nog's Mods server for helping out by setting up the access transformers and implementing some C++ fixes during the updating process. It saved me a lot of time. Also, big thanks to McGalleon for helping me test ContentLib functionality in U5 and checking over the docs.

## Removals (for now)

- CDO edits
  - All CDO edit functionality present in 0.1.2 of this mod is **not currently available.**
  - ~~I do not have access to the source code for the verion that has CDO edits implemented. I contacted Nog about it, and **I plan to bring them back later** once I have the source code.~~
  - I got the updated source code from Nog, but I haven't had the time to bring it in to this updated version yet. You can access the U4 version of CDO edit source code [here](https://github.com/Nogg-aholic/ContentLib/tree/cdo-stuff). **I'll bring this back as soon as I can.**

## New Stuff

- Easily select logging levels from the mod configs menu. Selecting a more detailed option includes previous options.
  - Errors and Sequence Only - Default, good for users playing with the mod
  - Warnings and Info - If you're developing stuff with ContentLib, you should *at least* be at this level
  - Debug - The most detailed logging, includes stuff like checked folder paths
- All C++ logs are now in the `LogContentLib` category so it's easier to track things down in the logs. Most Blueprint logs are prefaced with `[CL]`
- All content is now processed in the order: Vanilla, Mods, /Configs/ folder overrides.
  - Before, it was not consistent which were loaded first, mods or Configs overrides.
  - This change means that Configs folder overrides will always take precedence over anything else, as they should.
- Chat Commands hooked up again correctly
  - Note - **Hot reloading is still experimental so expect bugs**. The game wasn't really designed to work for it.
  - According to McGalleon, Patches and modifications to already-existing ContentLib-created content should work without a full game reload, but introducing new things may require a full game reload.
  - /CLItemReload - Hot reloads Items, Item Patches, Icons, and Visual Kits.
  - /CLRecipeReload - Hot reloads Recipes and Recipe Patches.
  - /CLSchemReload - Hot reloads Schematics and Schematic Patches.
  - /CLDumpVisualKits - Dumps Visual Kits to the ContentLib mod folder, for debugging. Vanilla and registered items are dumped. I have no idea if this still works, I just hooked it up again.
- Updated documentation
  - Docs now mention that patch files should be in the `<type>Patches/` folder instead of the usual folder, they had completley glossed over this detail in the past
    - ex. `ItemPatches/` vs `Items/`
- Additional warning features
  - Warnings will be printed to the console if you have a Patch formatted file in a non-Patch folder
  - A warning will be printed if you have a Liquid/Gas type item with a non-Fluid stack size, or vice versa
  - An error will be printed if you use an unrecognized StackSize or Form when defining an item via JSON
- JSON-generated Item Categories now automatically have their asset name (not the in-game name) start with `Cat_` for consistency with base game and identification.

## Bugfixes

- Provided warnings that will hopefully help avoid common mistakes made when writing JSONs. See the 'New Stuff' section for more details.
- Fixed ContentLib expecting JSONs to use the name `Liquid` when specifying the stack size of `Fluid`. If you have any custom items that are fluids, **make sure they are using StackSize `Fluid` now**.
- Fixed custom fluids having an initial stack size of Medium in newly placed buildings, probably caused by the above bug.
- Fixed the Form of `Invalid` not being accepted for item descriptors, if you ever want to do that
- Possibly fixed a crash with JSON-defined Schematic Categories that I don't think anyone has ran into yet up to this point

## Known Bugs

- Hot reloading of schematics does not seem to work at all
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet

## Important Changes for Past Users and Developers

WIP updated documentation can be found [here](https://docs.ficsit.app/contentlib/latest/index.html).

Check out the in-game mod config screen for some extra utilities and tools to help with writing ContentLib content.

### If you write JSONs for the `/Configs` folder

- The directory structure has been changed to be more informative and consistent with a packaged mod (so the transition is easier)
  - Now all folders are within the ContentLib folder within configs, ex. `Configs/ContentLib/Items`
- To see the full list of new directories, check your logs after going to the ContentLib configs screen in-game

Example directories:

Config Folder Overrides

```?
Configs/ContentLib/Items/
Configs/ContentLib/Icons/
Configs/ContentLib/Recipes/
Configs/ContentLib/RecipePatches/
Configs/ContentLib/ItemPatches/
Configs/ContentLib/Schematics/
Configs/ContentLib/SchematicPatches/
Configs/ContentLib/VisualKits/
Configs/ContentLib/CDOs/
```

Packed with Mod

```?
Mods/ModReferenceGoesHere/ContentLib/Items/
Mods/ModReferenceGoesHere/ContentLib/Icons/
Mods/ModReferenceGoesHere/ContentLib/Recipes/
Mods/ModReferenceGoesHere/ContentLib/RecipePatches/
Mods/ModReferenceGoesHere/ContentLib/ItemPatches/
Mods/ModReferenceGoesHere/ContentLib/Schematics/
Mods/ModReferenceGoesHere/ContentLib/SchematicPatches/
Mods/ModReferenceGoesHere/ContentLib/VisualKits/
Mods/ModReferenceGoesHere/ContentLib/CDOs/
```

### If you pack JSONs with your mod, but don't necessarily use ContentLib in the editor

- Your Content Lib related files mod should now go in `<Mod Reference>/ContentLib/` **instead of** `<Mod Reference>/Configs/` to avoid name confusion with the Unreal plugin `/Config` folder and the SML configs folder
- You can use the PluginSettings.ini `AdditionalNonUSFDirectories` directive to ensure that this folder is packaged with your mod, see the directions [here](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/Adding_Ingame_Mod_Icon.html#_setup) regarding adding a mod icon to learn how. Simply add another line for the ContentLib folder.

### If you use ContentLib BP/CPP functions (or you use ContentLib in the editor)

I suggest you read the above section as well.

- I probably™ haven't changed the signature of any of the functions, but if you find something that changed, please let me know so I can update this
- You **must** have source of the updated versions of the JsonStructs and ContentInspector mods as loaded plugins, else packaging will fail
  - JsonStructs source can be found [here](https://ficsit.app/mod/JsonStructs)
  - ContentInspector source can be found [here](https://ficsit.app/mod/ContentInspector)

Thank you for reading to the end of these patch notes! Have a great day.
