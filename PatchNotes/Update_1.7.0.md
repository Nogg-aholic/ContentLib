New `RequireItemValidation` recipe feature, improved recipe producer matching




****

## New Stuff

- Added `RequireItemValidation` field to the Recipes and Recipe Patches (thanks [Miirym](https://github.com/Nogg-aholic/ContentLib/pull/15)!)
  - False by default (to be backwards compatible)
  - When true, before a recipe is created or recipe patch is applied, the all ingredients and products will be checked to ensure they exist. If validation fails, no creation/modification will be performed.
  - When false, no validation is performed and creation/modification will always be performed. If an item happens to be invalid it will be skipped to enable the rest of the recipe to be created or patched. (Previous behavior)
- All valid recipe producers are now logged on startup to help with debugging and recipe creation.

## Fixed Stuff

- Fixed matching of production buildings in recipes. This means that the workaround of `manual` as a producer in Recipes is no longer required - you can directly reference `BP_WorkBenchComponent` or `BP_WorkshopComponent` now. (Thanks [Jarno](https://github.com/Nogg-aholic/ContentLib/pull/20)!)
  - Previously some definitley not valid buildings (ex. foundations) were accepted even though they didn't work - this should no longer be the case. Contact me on the Discord if this broke something for you.
- Fixed oversight in JSON schema causing any recipe with more than 1 product to produce a warning.
- Hopefully fix linux server crash when patching nuclear fuels with no waste item, can't test this any time soon. ([#18](https://github.com/Nogg-aholic/ContentLib/issues/18))

## Info for Developers

ContentLib relies on FindObject with the ANY_PACKAGE specifier
to implement the arbitrary class finding from string functionality.
ANY_PACKAGE is currently deprecated and a future update will change it to search the Asset Registry instead.
It is unknown if or how this will affect other mods using ContentLib's features.
The plan is to try and make the switch seamless.

## Known Bugs

See the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib/issues?q=label%3Abug).
