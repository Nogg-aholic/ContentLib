[Update 5] CDO support is back. Check the docs for more info on how to use them. Bugfixes related to item patching and logging.




## Summary

**Documentation can be found [here](https://docs.ficsit.app/contentlib/latest/index.html) instead of the mod page,** since it's easier to keep up to date and allow others to edit.
Feel free to contribute via Pull Requests or suggestions on the Discord.

## New Stuff

- **CDO Edits** are back, more info on the [docs](https://docs.ficsit.app/contentlib/latest/Features/CDOs.html) page for them.
- New chat command `/CLCDOReload` to attempt to hot reload CDOs

## Updated Stuff

- Configuration screen visuals improved by Nog

## Bugfixes

- Fixed a problem that prevented asset loading log messages from displaying on Warning/Debug logging level when there were no errors.

## For Developers

- Perform your own CDO modifications directly with `UCLCDOBPFLib::GenerateCLCDOFromString` or `UCLCDOBPFLib::EditCDO`. See the `CL_CDO_Module`` blueprint asset for examples.
- Renamed all sub-modules from `InitGame<ASSET>` to `CL_<asset>_Module` for consistency (they aren't Game Modules, they're Instance sub-modules)

## Known Bugs

- Item patches that do not specify a Form will change the item's type to invalid; this is fixed for 0.4.0
- Schematic categories do not work correctly; this is fixed for 0.4.0
- Schematics are processed in alphabetical order, so if you want a ContentLib defined schematic to depend on another one, you must have the *dependee* schematic file name come alphabetically before than the *depender*. An easy way to do this is with numbers at the start of the file name, ex. `Schem_001_Example` as the base, with `Schem_002_Example` depending on 001. This will be fixed eventually, but for now, this workaround should help. Thanks McGalleon!

(Old bugs below)

- Hot reloading of schematics is inconsistent
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet
