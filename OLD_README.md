This readme is outdated as of 1/30/2022 but I've kept it in the repo in case I forgot to copy the info here somewhere else.

# ContentLib

This mod allows users (and developers) to easily **create** custom recipes, items & schematics, or **modify** existing ones! Custom content of your own design is only a couple of JSON files away!

**Read the documentation [here](https://docs.ficsit.app/contentlib/latest/index.html) for more detailed info on how to use the mod.**

Questions? Suggestions? Join the [Nog's Mods Discord server](https://discord.gg/kcRmFxn89d) or leave an issue on the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib/issues) (slower response time)

## ContentLib: Items

**Creating your own Items** (or Liquids/Gasses) is as simple as creating a JSON file (schema provided below) and placing it in the `FactoryGame/Configs/ContentLib/Items` directory and launching the game.

If it doesn't exist, and you want to use it, create it!

You can also **modify existing Items** in a similar manner (see Patches section).

This mod's Item format supports basically everything a Item Blueprint created in Unreal Engine does because it turns the JSON into such a blueprint at runtime.

<details>
<summary> Expand for more info on ContentLib Items </summary>

### Custom Item Example

Creating a new item is this simple, as long as you're fine with re-using meshes.

Below is an example of a freshly created Item and a screenshot of it within the game.

```json
{
  "Name": "Gold Too",
  "VisualKit": "Desc_OreGold_C",
  "Description" : "This is a new Item which re-uses the visuals of Caterium Ore!"
}
```

![New Item in game](https://i.imgur.com/T7OC3vq.jpg)

Remember that you still need a recipe somewhere that produces the item, or the game will not load it.

### File Patching Example (Modify/Overwrite existing Items)

In addition to creating new items, you can also use _ContentLib Items_ to modify existing items, both base-game items and those added by other mods.

The syntax for the item is mostly the same, but you must put an additional line at the start of the file to specify what item to overwrite - the item's blueprint path, and **the file goes in the `FactoryGame/Configs/ItemPatches` directory instead.**

You can find this blueprint path from a [reference list](https://github.com/Goz3rr/SatisfactorySaveEditor/blob/master/Reference%20Materials/Resources.txt), via a tool like [UModel](https://www.gildor.org/en/projects/umodel), or by looking a mod's source code (either online or ingame with [ContentInspector](https://ficsit.app/mod/F3gKrk7S5nbSBN)).

Below is an example modification of the base game's "Caterium Ore" item, which is named OreGold internally. With the power of ContentLib Items, you can [reveal who's really behind that mask](https://i.imgur.com/eawzrXv.png) - Gold Ore!

This example replaces the item name and description, but you can modify other fields, too.

```json
//Game/FactoryGame/Resource/RawResources/OreGold/Desc_OreGold.Desc_OreGold_C
{
  "Name": "Gold Ore",
  "Description" : "Surely not Caterium Ore. Couldn't be!"
}
```

Remember, since this is modifying an item not defined by ContentLib, it goes in `/ItemPatches` instead of `/Items`.

You can check the JSON schema linked below to find out what fields are optional and which are required.

### Writing your Own Items

In order to write your own Items, you will need the following:

* A text editor, preferably one that supports JSON Schema validation (like Visual Studio Code or Rider)
* If you want to _re-use existing (modded or vanilla) item icons and meshes_, the internal name of the item whose Visual Kit you want to re-use.
* If you want to use your own _icons_, PNG images for the icons.
* If you want to use your own _meshes_, an packaged mod with those files contained within (the reasoning for this is explained further down).
* A way to define a recipe for obtaining your item, either via a mod, or via [ContentLib Recipes](https://ficsit.app/mod/5ak7eHymSNw4YN)

First, an explanation of the JSON schema, which documents the structure of the Items.

#### JSON Schema

The format of item definition JSON files follows a [JSON Schema](https://json-schema.org/), a helpful template that both explains the structure of the file and allows your favorite editors to automatically check your syntax for you. The schema itself is human-readable, so you can also read it to learn the Item file format.

You can use [Visual Studio Code](https://youtu.be/m30JiCuW42U), [any JetBrains editor](https://www.jetbrains.com/help/idea/json.html#ws_json_schema_add_custom), or more, to get automatic suggestions and see the documentation as you write. Those links go to documentation on how to set it up.

**View the Item json schema on GitHub [here](https://raw.githubusercontent.com/Nogg-aholic/ContentLib_Items/master/FContentLib_Item.json).**

#### Visual Kits

_ContentLib Items_ introduces the concept of Visual Kits to prevent having to deal with long, verbose blueprint paths in Item definitions.

Visual Kits are automatically generated at Runtime for every pre-existing Item (including those from other mods) and can be accessed by using the ClassName of the item.

Below is an example Visual Kit that reuses properties of the base-game Water resource.

```json
{
  "Mesh": "/Game/FactoryGame/Resource/Parts/PackagedWater/SM_PackedWater_01.SM_PackedWater_01",
  "BigIcon": "/Game/FactoryGame/Resource/RawResources/Water/UI/LiquidWater_Pipe_512.LiquidWater_Pipe_512",
  "SmallIcon": "/Game/FactoryGame/Resource/RawResources/Water/UI/LiquidWater_Pipe_256.LiquidWater_Pipe_256",
  "FluidColor":
  {
    "r": 122,
    "g": 176,
    "b": 212,
    "a": 255
  },
  "GasColor":
  {
    "r": 122,
    "g": 176,
    "b": 212,
    "a": 255
  }
}
```

As with the Item schema, all Fields are optional.

If a Json file using this Schema is placed in the Folder `FactoryGame/Configs/ContentLib/VisualKits`, the name of this file can be used by all Item definitions, and is preferred over any existing definitions of the same name (meaning base-game visual kits will be overwritten).

If this file was named `CustomWater.json`, then the Visual Kit could be used in Item definitions via the name "CustomWater".

**View the VisualKit json schema on GitHub [here](https://raw.githubusercontent.com/Nogg-aholic/ContentLib_Items/master/FContentLib_ItemVisualKit.json).**

#### Writing Your Items

After setting up your text editor, go ahead and copy in the "Example Starter Item" from above.

Next, you need to find the ClassName of the exiting item you want to copy. These names are part of the 'blueprint path' of the items in the game's files. Thankfully, this mod will automatically resolve part of the blueprint path into the full name, saving you from having to type out the full name. You can exclude the "_C" from the end as well. More info on how this works can be found in the "The Inner Workings" section.

You can find these blueprint path from a [reference list](https://github.com/Goz3rr/SatisfactorySaveEditor/tree/master/Reference%20Materials), via a tool like [UModel](https://www.gildor.org/en/projects/umodel), browsing around in the SML starter project, or by looking a mod's source code (either online or ingame with [ContentInspector](https://ficsit.app/mod/F3gKrk7S5nbSBN)).

Use the values you've selected and the example item above to define your own Item, then place the json file in the `FactoryGame/Configs/Items` directory.

You have created an item, but there is no way to obtain it yet. You can use [ContentLib Recipes](https://ficsit.app/mod/5ak7eHymSNw4YN) to easily create recipes with which your Items can be obtained.

Now that you have an item and a recipe to obtain it with, launch the game and check it out.

If you've done everything correctly, you should see your Item.

#### Tracking Down Errors

If an error is encountered while loading a Items, the mod will print error information to the SML console and your log files. To see the SML console, turn on `consoleWindow` in your configs. You can find info on how to do that [here](https://docs.ficsit.app/satisfactory-modding/latest/SMLConfiguration.html). You can find your log files in your Local AppData directory, ex. `AppData\Local\FactoryGame\Saved\Logs`.

#### Why do I need to package my own mod to use custom textures and meshes?

Unreal Engine expects materials and meshes to be in a very specific cooked format in order for use. This preparation step is unavoidable. The simplest way to prepare your files for usage is to package them using the same method used to package a mod.

#### Why do I need to use [ContentLib Recipes](https://ficsit.app/mod/5ak7eHymSNw4YN) so that I can obtain my item? Can't I just spawn it in?

As a performance measure, an Item is generally not available in Satisfactory unless a Recipe exists to register it. When you create a new item with _ContentLib Items_, there are no Recipes that produce it, so the game does not know to register it as existing. Writing a recipe with [ContentLib Recipes](https://ficsit.app/mod/5ak7eHymSNw4YN) is the simplest way to create a recipe for it. Depending on how your item spawning mod locates items, it may or may not be able to find this otherwise hidden item and register it for you.

#### Why do Item overwrites need the blueprint path?

The reason is simple:

* This Item may not be loaded when it isn't added by a Registered Recipe at this point.
* There is no way to "Find" it by name without loading *everything*.

Therefore, this mod uses a Blueprint Path here to reliably load the Item to be Patched.

#### Why is the blueprint path not part of the Json itself?

* The step of turning the raw text into JSON is skipped when the item class fails to load to improve performance.
* Putting the path inside of the JSON would have forced this Mod to do the conversion step earlier.
* Even if it were part of the JSON, it has no actual value for the Items themselves, just for the mod figuring out what to overwrite. The path resolves to either a useless nullptr or a valid pointer with no further usefulness.

### For Mod Developers

You can use  _ContentLib Items_ to define the Items for your own mods.
You can use  [_ContentLib Recipes_](https://ficsit.app/mod/5ak7eHymSNw4YN) to define the Recipes for your own mods.

This makes it easy for end users to configure recipes to their own balance preferences, and can help you quickly create recipes or Items if you want to automatically generate them from another source.

You can follow the examples below to load recipes or Items from your mod's own directories, or create and register recipes from strings alone, or whatever you'd like!

Make sure that you list `ContentLib` as a required dependency of your own mod, and follow the directions [here](https://docs-dev.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/Adding_Ingame_Mod_Icon.html) to be sure that any json files you include in your mod files get packed into your final mod.
</details>

## ContentLib: Recipes

Creating your own recipes is as simple as creating a JSON file
(schema provided below) and placing it in the `FactoryGame/Configs/ContentLib/Recipes`
or the `FactoryGame/Configs/ContentLib/RecipePatches` directory and launching the game.

You can also overwrite any recipe in a very similar manner.

If it doesn't exist, and you want to use it, create it!

This mod's recipe format supports basically everything a recipe Blueprint created in Unreal Engine does because it turns the JSON into such a blueprint at runtime.

<details>
<summary> Expand for more info on ContentLib Recipes </summary>

### Example Recipe

Below is an example recipe and a screenshot of it within the Item Codex in game.

```json
{
  "Name": "BioFuel",
  "Ingredients": [
    {
      "Item": "Coal",
      "Amount": 1
    }
  ],
  "Products": [
    {
      "Item": "Biofuel",
      "Amount": 10
    }
  ],
  "ManufacturingDuration": 1,
  "ProducedIn":["ConstructorMk1"],
  "UnlockedBy":["Schematic_1-1"]
}
```

![Recipe](https://i.imgur.com/ZUl6Mc5.png "Recipe")

### Example Complex Recipe

Here's an example recipe for the Blender made by McGalleon.

```json
{
  "Name": "Synthetic Crystal",
  "Ingredients": [
    {
      "Item": "Desc_RawQuartz",
      "Amount": 16
    },
    {
      "Item": "Desc_QuartzCrystal",
      "Amount": 7
    },
    {
      "Item": "Desc_Water",
      "Amount": 4800
    }
  ],
  "Products": [
    {
      "Item": "Desc_QuartzCrystal",
      "Amount": 20
    }
  ],
  "ManufacturingDuration": 8,
  "ProducedIn": ["Build_Blender"],
  "UnlockedBy": ["Schematic_1-1"]
}
```

![Blender example ingame](https://cdn.discordapp.com/attachments/771801486828896260/863510909476143184/unknown.png)

### File Patching Example (Overwrite other recipes)

In addition to creating new recipes, you can also use _ContentLib Recipes_ to modify existing recipes, both base-game recipes and those added by other mods.

The syntax for the recipe is the exact same, but you must put an additional line at the start of the file to specify what recipe to overwrite - the recipe's blueprint path.

You can find this blueprint path from a [reference list](https://github.com/Goz3rr/SatisfactorySaveEditor/blob/master/Reference%20Materials/Recipes.txt), via a tool like [UModel](https://www.gildor.org/en/projects/umodel), or by looking a mod's source code (either online or ingame with [ContentInspector](https://ficsit.app/mod/F3gKrk7S5nbSBN)).

Below is an example modification of the base game's Biofuel recipe.

**Important note --**
Different to Recipes that should be created, Recipe Patches belong into a Folder with this Path: `FactoryGame/Configs/RecipePatches`

```json
//Game/FactoryGame/Recipes/Constructor/Recipe_Biofuel.Recipe_Biofuel_C
{
  "Name": "BioFuel Override",
  "Ingredients": [
    {
      "Item": "Coal",
      "Amount": 1
    }
  ],
  "Products": [
    {
      "Item": "Biofuel",
      "Amount": 10
    }
  ]
}
```

### Writing Your Own Recipes

#### Example Starter Recipe

Most of your recipes will follow the JSON structure below:

```json
{
  "$schema": "https://raw.githubusercontent.com/Nogg-aholic/ContentLib_Recipes/master/FContentLib_Recipe.json",
  "Ingredients": [
    {
      "Item": "Coal",
      "Amount": 1 
    }
  ],
  "Products": [
    {
      "Item": "Coal",
      "Amount": 1
    }
  ],
  "ManufacturingDuration": 1,
  "ProducedIn": [
    "Build_ConstructorMk1"
  ],
  "UnlockedBy": [
    "Schematic_1-1"
  ]
}
```

You can check the JSON schema linked above to find out what fields are optional and which are required. There are a lot more fields on offer, such as the variable power settings for the Particle Accelerator.

For example, you could leave out the `UnlockedBy` field, but the recipe would not be accessible without some other code to register it. An example of a minimal recipe is presented below.

<details>
<summary> Minimum valid recipe (still needs registration by another mod) </summary>

```json
{
  "Ingredients": [
    {
      "Item": "Coal",
      "Amount": 1 
    }
  ],
  "Products": [
    {
      "Item": "Coal",
      "Amount": 1
    }
  ],
  "ProducedIn": [
    "Build_ConstructorMk1"
  ]
}
```

</details>

In order to write your own recipes, you will need the following:

1. A text editor, preferably one that supports JSON Schema validation (like Visual Studio Code or Rider)
1. The internal names of the ingredients and products in your recipe
1. The internal name of the building(s) that can use your recipe
1. The internal name of the schematic that will unlock your recipe

First, an explanation of the JSON schema, which documents the structure of the recipes.

#### JSON Schema

The format of recipe json files follows a [JSON Schema](https://json-schema.org/), a helpful template that both explains the structure of the file and allows your favorite editors to automatically check your syntax for you. The schema itself is human-readable, so you can also read it to learn the recipe file format.

You can use [Visual Studio Code](https://youtu.be/m30JiCuW42U), [any JetBrains editor](https://www.jetbrains.com/help/idea/json.html#ws_json_schema_add_custom), or more, to get automatic suggestions and see the documentation as you write. Those links go to documentation on how to set it up.

**View the Recipe json schema on GitHub [here](https://raw.githubusercontent.com/Nogg-aholic/ContentLib_Recipes/master/FContentLib_Recipe.json).**

#### Writing Your Recipe

After setting up your text editor, go ahead and create a new file called `Recipe_YourNameHere.json` and copy in the "Example Starter Recipe" from above.

It is important that your recipe name is formatted in this way because the file name will become the internal recipe blueprint name once the recipe is loaded.

Override names do not need to follow any specific pattern, as they are modifying an existing asset as opposed to creating a new one.

Next, you need to find the item, building, and schematic names. These names are part of the 'blueprint path' of the items in the game's files. Thankfully, this mod will automatically resolve part of the blueprint path into the full name, saving you from having to type out the full name. You can exclude the "_C" from the end as well. More info on how this works can be found in the "The Inner Workings" section.

You can find these blueprint path from a [reference list](https://github.com/Goz3rr/SatisfactorySaveEditor/tree/master/Reference%20Materials), via a tool like [UModel](https://www.gildor.org/en/projects/umodel), browsing around in the SML starter project, or by looking a mod's source code (either online or ingame with [ContentInspector](https://ficsit.app/mod/F3gKrk7S5nbSBN)).

Watch out - if you're using fluids in your recipes, you need to multiply the item count by 1000,  Each fluid 'item' in Satisfactory is small interval, which allows for decimal values of fluid in-game. For example, if you want a recipe to consume 4.8 Fuel per operation, you will need to use `Item` name `Desc_LiquidFuel` and an `Amount` of 4800.

Here are a few Vanilla building names:

* Constructor : Build_ConstructorMk1
* Assembler : Build_AssemblerMk1
* Foundry : Build_FoundryMk1
* Manufacturer : Build_ManufacturerMk1
* Blender: Build_Blender

Schematic names for HUB milestones will generally follow the format `Schematic_1-1`, but you can find out specific ones via the approaches described above.

Use the values you've selected and the example recipe above to create your recipe, then place it in the `FactoryGame/Configs/ContentLib/Recipes` directory and launch the game to try it out. If you've done everything correctly, you should see your recipe unlocked by the schematic you specified, and you can use it in the machines you added to ProducedIn.

#### Tracking Down Errors

If an error is encountered while loading a recipe, the mod will print error information to the SML console and your log files. To see the SML console, turn on `consoleWindow` in your configs. You can find info on how to do that [here](https://docs.ficsit.app/satisfactory-modding/latest/SMLConfiguration.html). You can find your log files in your Local AppData directory, ex. `AppData\Local\FactoryGame\Saved\Logs`.

##### Common Errors

* Nothing showing up at all in the logs
  * Make sure your recipes are in the `FactoryGame/Configs/ContentLib/Recipes` directory.
* `Wrong Naming Convention ! "Recipe" expected followed by a Name`
  * Your recipe file should be names like this: `Recipe_YourNameHere.json`

### The Inner Workings

#### How does resolving Item names work?

Items are resolved by comparing the supplied Item name against existing Item ClassNames in the game. The first match the mod finds will be used.

Watch out for ambiguity with Class Names! Sometimes items can have very similar names. Be as specific as possible with the name to avoid problems with this.

Class Names have a '_C' suffix when they have been generated by a Blueprint. The "_C" suffix, as well as the naming-convention related "Build" or "Desc" prefix, is optional for this mod.

#### Why do recipe overwrites need the blueprint path?

The reason is simple:

* This Recipe may not be loaded when it isn't added by a Registered Schematic at this point.
* There is no way to "Find" it by name without loading *everything*.

Therefore, this mod uses a Blueprint Path here to reliably load the Recipe to be Patched.

#### Why is the blueprint path not part of the Json itself?

* The step of turning the raw text into JSON is skipped when the recipe class fails to load to improve performance.
* Putting the path inside of the JSON would have forced this Mod to do the conversion step earlier.
* Even if it were part of the JSON, it has no actual value for the Recipes themselves, just for the mod figuring out what to overwrite. The path resolves to either a useless nullptr or a valid pointer with no further usefulness.

### For Mod Developers

You can use  _ContentLib Recipes_ to define the recipes for your own mods.

This makes it easy for end users to configure recipes to their own balance preferences, and can help you quickly create recipes if you want to automatically generate them from another source.

`Mods/<Mod reference>/ContentLib/Recipes`

`Mods/<Mod reference>/ContentLib/RecipePatches`

are also included in the Search for Files. That means you can bundle your Recipes and Patches into a Mod without any Pak or Binary Files!

You can follow the examples below to load recipes from your mod's own custom directories, or create and register recipes from strings alone, or whatever you'd like!

Make sure that you list `ContentLib` as a required or optional dependency of your own mod, and follow the directions [here](https://docs-dev.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/Adding_Ingame_Mod_Icon.html) to be sure that any json files you include in your mod files get packed into your final mod.

![Guide](https://i.imgur.com/p5TgndI.png "Guide")

</details>

## ContentLib: Schematics

You can use _ContentLib_ to create your own schematics, or edit existing ones!

This section is a WIP.

To specify schematic icons, use item Visual Kits.

## Class Default Object Edits

**NOTE- These are not working in the current version of the mod since I (Rob) lack the source code for that version.** You can also use ContentLib to perform (Class Default Object) CDO edits.

<details>
<summary> Expand for more info on ContentLib CDO Edits </summary>

Path for CDO Files : `Configs/ContentLib/CDO`

### Examples

```json
{
  "Class": "/Game/FactoryGame/Resource/Parts/GoldIngot/Desc_GoldIngot.Desc_GoldIngot_C",
  "Edits": [
    {
      "Property": "mDisplayName",
      "Value": "Gold Ingot"
    },
    {
      "Property": "mInventoryIcon",
      "Value": {
        "ImageSize": {
          "X": 255,
          "Y": 255
        },
        "Margin": {
          "Left": 0,
          "Top": 0,
          "Right": 0,
          "Bottom": 0
        },
        "TintColor": {
          "SpecifiedColor": {
            "R": 1,
            "G": 1,
            "B": 1,
            "A": 2
          },
          "ColorUseRule": 0
        }
      }
    }
  ]
}
```

```json
{
  "Class": "/Game/FactoryGame/Recipes/Blender/Recipe_FusedModularFrame.Recipe_FusedModularFrame_C",
  "Edits": [
    {
      "Property": "mIngredients",
      "Value": [
        {
          "ItemClass": "/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C",
          "Amount": 643
        },
        {
          "ItemClass": "/Game/FactoryGame/Resource/Parts/AluminumCasing/Desc_AluminumCasing.Desc_AluminumCasing_C",
          "Amount": 245
        },
        {
          "ItemClass": "/Game/FactoryGame/Resource/RawResources/NitrogenGas/Desc_NitrogenGas.Desc_NitrogenGas_C",
          "Amount": 123
        }
      ]
    }
  ]
}
```

```json
{
  "Class": "/Game/FactoryGame/-Shared/Material/MI_Factory_Base_01.MI_Factory_Base_01",
  "Edits": [
    {
      "Property": "Parent",
      "Value": "/AdaptingGenerators/Assets/MM_FactoryCopy.MM_FactoryCopy"
    }
  ]
}
```

</details>

Icons by deantendo#4265
