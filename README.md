# ContentLib

This mod allows users (and developers) to easily **create** custom recipes, items & schematics, and more, or **modify** existing ones from any mod.
Custom content of your own design is only a couple of JSON files away!
Developers can also leverage the ContentLib API to generate or modify content at runtime.

**Read [the documentation](https://docs.ficsit.app/contentlib/latest/index.html) for more detailed info on how to use the mod.**

Questions? Suggestions? Join the [Nog's Mods Discord server](https://discord.gg/kcRmFxn89d) or leave an issue on the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib/issues) (slower response time)

_If you enjoy my work, please consider donating to my [completely optional tip jar](https://ko-fi.com/robb4)._

## Multiplayer Support

ContentLib itself supports multiplayer and dedicated servers as long as all sides have the same content definitions.
Other mods that depend on ContentLib may not support multiplayer, so check the mod page or ask the mod author.

## Examples

You can use ContentLib for your own personal edits, but you can also easily distribute your scripts as mods for others to use.

To get an idea of what ContentLib is capable of, check out a few existing mods that use ContentLib:

<!-- cspell:disable -->
- [RePan - Oil](https://ficsit.app/mod/RePan_Petroleum) by McGalleon
- [Recipe Maker](https://ficsit.app/mod/RecipeMaker), [Auto Packing](https://ficsit.app/mod/AutoPacking), [Building Kits](https://ficsit.app/mod/BuildingKits), [Hide Researched Milestones](https://ficsit.app/mod/HideSchematic), [Recipe Randomizer](https://ficsit.app/mod/RecipeRandomizer), and more by Andre Aquilla
- [ContentInfo](https://ficsit.app/mod/ContentInfo) by Nog
- [Useful Liquid Biofuel](https://ficsit.app/mod/UsefulLiquidBiofuel) by Jarno458
- [No Package For Fluid](https://ficsit.app/mod/Jimmon_NoPackageFluid) by jimmon89
- [Ethical Power Slug Treatment](https://ficsit.app/mod/EthicalPowerSlugTreatment) by LordOcram
<!-- cspell:enable -->

If you'd like to do this, check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

## ContentLib: Items

**Creating your own Items** (or Liquids/Gasses) is as simple as creating a JSON file, placing it in the correct directory, and launching the game.

You can also **modify existing Items** from any mod in a similar manner.

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

### Custom Item Example

Creating a new item is this simple, as long as you're fine with re-using meshes.

Below is an example of a freshly created Item and a screenshot of it within the game.

```json
{
  "$schema": "https://raw.githubusercontent.com/budak7273/ContentLib_Documentation/main/JsonSchemas/CL_Item.json",
  "Name": "Gold Too",
  "VisualKit": "Desc_OreGold_C",
  "Description" : "This is a new Item which re-uses the visuals of Caterium Ore!"
}
```

![New Item in game](https://i.imgur.com/T7OC3vq.jpg)

Remember that you still need a recipe somewhere that produces the item, or the game will not load it.

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

### Item Patching Example

In addition to creating new items, you can also use _ContentLib_ to modify existing items, both base-game items and those added by other mods.

Below is an example modification of the base game's "Caterium Ore" item, which is named OreGold internally. With the power of ContentLib Items, you can [reveal who's really behind that mask](https://i.imgur.com/eawzrXv.png) - Gold Ore!

This example replaces the item name and description, but you can modify other fields, too.

```json
//Game/FactoryGame/Resource/RawResources/OreGold/Desc_OreGold.Desc_OreGold_C
{
  "$schema": "https://raw.githubusercontent.com/budak7273/ContentLib_Documentation/main/JsonSchemas/CL_Item.json",
  "Name": "Gold Ore",
  "Description" : "Surely not Caterium Ore. Couldn't be!"
}
```

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

## ContentLib: Recipes

Creating your own recipes is as simple as creating a JSON file, placing it in the correct directory, and launching the game.

You can also **modify existing recipes** in a very similar manner.

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

### Example Complex Recipe

Here's an example recipe for the Blender made by McGalleon.

```json
{
  "$schema": "https://raw.githubusercontent.com/budak7273/ContentLib_Documentation/main/JsonSchemas/CL_Recipe.json",
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

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

### Recipe Patching Example

In addition to creating new recipes, you can also use _ContentLib_ to modify existing recipes, both base-game recipes and those added by other mods.

Below is an example modification of the base game's Biofuel recipe. The recipe display name is changed to `BioFuel Override` and the recipe now consumes 1 Coal to produce 10 Biofuel instead.

```json
//Game/FactoryGame/Recipes/Constructor/Recipe_Biofuel.Recipe_Biofuel_C
{
  "$schema": "https://raw.githubusercontent.com/budak7273/ContentLib_Documentation/main/JsonSchemas/CL_Recipe.json",
  "Name": "BioFuel Override",
  "Ingredients": [
    {
      "Item": "Desc_Coal",
      "Amount": 1
    }
  ],
  "Products": [
    {
      "Item": "Desc_Biofuel",
      "Amount": 10
    }
  ]
}
```

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

## ContentLib: Schematics

You can use _ContentLib_ to create your own schematics, or edit existing ones!

This works for HUB Milestones, MAM Nodes, and AWESOME Shop items.

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

## Class Default Object Edits

You can also use ContentLib to perform (Class Default Object) CDO edits.

This is an advanced feature that lets you modify a wide variety of fields on basically any asset, but is not without limitations.

Check out the [docs](https://docs.ficsit.app/contentlib/latest/index.html) to get started.

## Credits

Icons by deantendo#4265
