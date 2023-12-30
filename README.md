# SvgBooga 
A free and open source SVG importer for Unreal Engine 5

## Getting Started
Simply `git clone` this repository into your `Plugins` directory or download it as an archive and extract to `Plugins`.

## Usage
![step1](https://github.com/etodanik/SvgBooga/assets/1970424/9297d7a3-f68f-439b-ab90-6a4a0771e785)
![step2](https://github.com/etodanik/SvgBooga/assets/1970424/1f9b5c19-3bb2-4289-880f-6ff94a4ee4c9)
![step3](https://github.com/etodanik/SvgBooga/assets/1970424/72955d05-4eab-482c-9a3a-84be7c63c9b4)

## Planned Features
* Rendering the SVG as a Signed Distance Field texture to save on space
* Allowing the creation of an SVG texture from XML
* Editing an imported SVG inline via XML
* Picking a custom background (rather than transparent)

If you come to need any of the above urgently or any other feature, open a GitHub issue for it.

## Known Issues
* If you try to reimport not the asset itself, but the texture inside it, it will crash the Unreal Editor
* Reimport is only possible by going into the editor for the asset, right clicking from content editor has "Reimport" grayed out
