# ContentLib Docs

## ROBNOTE I copied all these out to the other docs now, I'm pretty sure

These docs are still highly a work in progress. I decided that if I waited until finishing these to release the U5 update, it would be months before anyone got their hands on the mod, and these docs are only really relevant to a smaller portion of the ContentLib userbase.

If you have any suggestions for these docs, or are willing to contribute, please open a Pull Request or contact me (Robb) on the [Nog's Mods discord server](https://discord.gg/kcRmFxn89d).

## Debugging your Scripts

Although ContentLib is generally easier to use than setting up stuff in the Unreal editor, there are still room for mistakes. You should follow the steps below to set up for debugging your scripts.

- In your SML configs, you should turn on the `consoleWindow` config option so you can see the debug console while the game is running. Find out how to do this [here](https://docs.ficsit.app/satisfactory-modding/latest/SMLConfiguration.html).
- It can be beneficial to open the `FactoryGame.log` file in a text editor that has good searching capabilities, such as Notepad++, so you can track down messages. The file is stored in your `%localappdata%` folder, for example, `....\AppData\Local\FactoryGame\Saved\Logs\FactoryGame.log`
- You should get the **Content Inspector** mod to check if your content has loaded correctly, and to discover values and paths of base game and mod assets. You can download it [here](https://ficsit.app/mod/F3gKrk7S5nbSBN).
- Increase your Logging Detail Level to either `Warnings and Info` or `Debug` in the Mod Configs screen for ContentLib

Once you have these tools set up, the following tips/process should help you debug.

- Check the logs for WARNING and ERROR level messages in the `LogContentLib` category, or containing the text `[CL]`, these contain potentially useful messages.
- Use Content Inspector (mentioned above) to check the data for loaded items and see if it matches what you expect

- Although ContentLib attempts to offer live reloading of JSON assets (when triggered by the chat commands or config widget) this hasn't really been tested much, and Satisfactory was not designed for it.

## Frequent Problems

- I created a new Item but it doesn't exist in-game
  - Remember that you need to have a Recipe that produces that item, and a Schematic to unlock that item, for it to exist in-game.
  - Make sure that you put your files in the correct folders. See the "Where to put your ContentLib files" section.
- I wrote a Patch, but it isn't changing anything!
  - Make sure that you put your files in the correct folders. See the "Where to put your ContentLib files" section.

## Creating New Content via JSON

This section applies to you if you want to:

- Create a new item, recipe, or schematic
- Perform CDO edits

## Modifying Existing Content via JSON

This section applies to you if you want to:

- Edit an existing item, recipe, or schematic
- Perform CDO edits

## Where to put your ContentLib files

While testing out the mod, or writing scripts for your own use, you should put your ContentLib files in the following locations within your game install's Configs folder, with your other mod config files.

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

If you're packaging your scripts into a mod, see the "Packaging ContentLib files for distribution with a mod" section

## Packaging ContentLib files for distribution with a mod

You don't necessarily need to set up a full modding project (Unreal editor install), but it helps a lot since it will package your mod for you, and it will make it easier to transition to a full mod later, if you so desire.

### Option 1 - No Unreal Editor

Before you start this section, I highly suggest going with Option 2 instead, since it makes it a lot easier to ensure the folder structure is correct, and helps if you ever decide to go further in modding.

Simply start out with this template plugin and edit the files to match your needs TODO.

Put the folder in the `/Mods/` folder of your Satisfactory game install and rename it to the [Mod Reference](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/index.html#_mod_reference) you want to use for your mod. Be sure to view the linked page to see what characters are supported in mod references.

Then rename the `.uplugin` file inside the folder to match the folder name (again, the mod reference).

TODO

Now continue below at "Moving in the scripts"

### Option 2 - With Unreal Editor

You'll need to follow the normal modding setup docs [here](https://docs.ficsit.app/) to get a copy of the editor.

Create your plugin as normal as described on the docs.

Now continue below at the "Moving in the scripts" section. Your script files will go in your modding project's plugin folder. In order to package your changes for the game, and for distribution as a mod, use Alpakit as described in the docs.

You can still edit the json files in your `/Mods/` folder, but if you chose to do so, be careful not to accidentally lose their contents by packing over them with Alpakit.

### Moving in the scripts

Once you have decided to put your ContentLib scripts into a mod, you can move them out of your Configs folder and into the plugin folder for your mod.

Basically, copy the ContentLib subfolder from your Configs into your plugin's root directory. To be specific, they should now be in the following locations:

```?
/ModReferenceGoesHere/ContentLib/Items/
/ModReferenceGoesHere/ContentLib/Icons/
/ModReferenceGoesHere/ContentLib/Recipes/
/ModReferenceGoesHere/ContentLib/RecipePatches/
/ModReferenceGoesHere/ContentLib/ItemPatches/
/ModReferenceGoesHere/ContentLib/Schematics/
/ModReferenceGoesHere/ContentLib/SchematicPatches/
/ModReferenceGoesHere/ContentLib/VisualKits/
/ModReferenceGoesHere/ContentLib/CDOs/
```

Your mod needs to list ContentLib as a dependency, follow the example [here](https://docs.ficsit.app/satisfactory-modding/latest/UploadToSMR.html#_uplugin_file) to see how to list mods as dependencies of other mods. Alternatively, look at the `uplugin` of another released mod that depends on ContentLib, such as [RePan](https://ficsit.app/mod/RePan_Petroleum), to see how to format it.

You need a special file to tell UnrealPak that the ContentLib directory should be packed with your mod.
`/<mod reference>/Config/PluginSettings.ini`

```ini
[StageSettings]
+AdditionalNonUSFDirectories=Resources
+AdditionalNonUSFDirectories=ContentLib

```

### Testing before Releasing

TODO

### Uploading

This process is out of scope of this tutorial. You can follow the directions in the [modding documentation](https://docs.ficsit.app/satisfactory-modding/latest/UploadToSMR.html) for uploading to Satisfactory Mod Repository.

A couple things to note

- Made sure you followed the directions above for including

## ContentLib APIs (advanced usage)

ContentLib also has some capabilities not accessible via JSON, and useful for creating mods of your own. These include:

- Easily processing (and modifying) all loaded recipes or items in the game
- Runtime content generation (ex. generate new recipes for every fluid in the game, like Aquilla does in the mod [Free Fluids - CL](https://ficsit.app/mod/A4WLK29kjJ54nm))
- Reverse lookup (ex. "What recipes can I use to make this item?")
- Recipe power cost analysis

To use these features, you will need to set up a full modding project by following the [Getting Started Guide](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/index.html) and bring in the ContentLib, JsonStructs, and ContentInspector plugins to your project.

Ask on the Discord for further help doing this, since these docs are still a work in progress.
