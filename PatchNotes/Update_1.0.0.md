Update 6 support. Significantly more documentation and tutorials. Improved JSON schemas. Schematic functionality bugfixes. CDOs are back!




## New Stuff

- Updated [documentation](https://docs.ficsit.app/contentlib/latest/index.html) that walks through setting up a dev workspace and creating most asset types
- JSON schemas for all asset types, which can be found [on the docs github](https://github.com/budak7273/ContentLib_Documentation/tree/main/JsonSchemas)
- Since Schematic Category logic is fixed, AWESOME Shop schematics now work. Example:

```json
{
  "$schema": "https://raw.githubusercontent.com/budak7273/ContentLib_Documentation/main/JsonSchemas/CL_Schematic.json",
  "Name": "Buy Leaves",
  "Type": "ResourceSink",
  "Category": "SC_RSS_Parts",
  "SubCategories": [
    "SC_RSS_Biomass"
  ],
  "Description": "The AWESOME Shop actually displays the description of the schematic.",
  "Cost": [
    {
      "Item": "Desc_ResourceSinkCoupon",
      "Amount": 1
    }
  ],
  "ItemsToGive": [
    {
      "Item": "Desc_Leaves",
      "Amount": 200
    }
  ]
}
```

- Schematic `ClearSubCategories` field works now, it previously did nothing
- Schematic `ClearDeps` field works now, it previously did nothing
- Improved logging (ex. warn when a value was specified for a field but it does not appear to have applied to the final item)

## Changed Stuff

- Updated to version `1.0.0` so that other mods can depend on this mod using `^` without weird behaviors
- Changed a few field names:
  - Recipe `Category` field is now `OverrideCategory` to better reflect its purpose. Recipes default to inheriting the category of their product.
  - Schematic `Cat` field is now `Category` for consistency
  - Schematic `SubCat` field is now `SubCategories` to better indicate that it's an array type
  - Schematic `ClearCats` field is now `ClearSubCategories` to clarify it only affects Sub Categories
  - If you use any of these old fields, ContentLib will complain in the logs but still port it to the new field, for now.

## Bugfixes

- Fixed that Schematic Categories and Sub Categories would not register or lookup correctly
- Fixed that the default Form for a CLItem was Invalid, meaning that item patches would change Items to the Invalid form type if not explicitly specified in the patch.

## For Developers

- Removed the Bind on BP Function definition that shared the exact same name as the SML one (this is the mod it was copied from when implemented in SML). This has caused me significant debugging pain in the past...
- A few BP lib functions may have changed name (such as `AddSlotsToUnlock` to `AddInventorySlotsToUnlock` during my clean up)
- You might need to update BP code that references the old field names (see Changed Stuff section)
- Schematic Categories now internally start with the prefix `Cat_` instead of `SC_`. This change was made because base-game uses `SC` as a term for a sub category.
- Content Lib Subsystem no longer keeps separate records of build/schematic/item categories, they are now all held in one collection called mCategories

## Known Bugs

Nothing new in this update, yet.

(Old bugs below)

- Schematics are processed in alphabetical order, so if you want a ContentLib defined schematic to depend on another one, you must have the *dependee* schematic file name come alphabetically before than the *depender*. An easy way to do this is with numbers at the start of the file name. This will be fixed eventually, but for now, this workaround should help. Thanks McGalleon!
- Hot reloading of schematics does not seem to work at all
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet
