# Changelog for Field-Map-Editor

All notable changes to the Field-Map-Editor project will be documented in this file, based on the available tags from the GitHub repository. Versions are sorted from the largest to the smallest number, with Canary representing the unreleased changes.

## [1.0.0.34] - HotFix

### Added
- Integration with GitHub milestones for release tracking (inspired by FFNX project).
- `changelog.md`
- `skip_branch_with_pr: true` to `appveyor.yml`
- Search to toml editor.
- Button to fix name(s) in search.
- Checkboxes to select which names need fixed.
- Parent checkboxs to toggle child checkboxes.
- Store previous name in toml.
- Undo Rename button.

### Changed
- Removed bullet for the Goto button.
- Optimized gizmo rendering for smoother interactions.
- Improved async texture loading with better thread synchronization.
- Updated shader pipeline for compatibility with newer OpenGL versions.
- Swapped Appveyor for Github Actions
- Swapped Conan for VCPKG
- Support Searching by Map Index and show Map Index/ID in dropdown.

### Fixed
- Fixed memory leaks in texture caching system.
- Resolved intermittent crashes during rapid field switching.
- Major rendering bug. Not clearing texture slots when flushing vertice buffer.

**Ongoing Milestone: Bugfix Release Sept-2025**
- Associated Items: Specific issues assigned to this milestone are not detailed in the available page content. For the current list of open and closed issues, please refer to the milestone page on GitHub: https://github.com/Sebanisu/Field-Map-Editor/milestone/1. As the milestone is active, the items may include ongoing bug fixes and improvements targeted for the September 2025 release.

**Related Issues & PRs:** 
- https://github.com/Sebanisu/Field-Map-Editor/pull/182 - Add change.log from grok, and add skip_branch_with_pr: true
- https://github.com/Sebanisu/Field-Map-Editor/pull/184 - Add button to allow fixing names in `deswizzle.toml`
- https://github.com/Sebanisu/Field-Map-Editor/pull/201 - Remove appveyor and conan, Add Github Actions and VCPKG
- https://github.com/Sebanisu/Field-Map-Editor/pull/203 - Support Searching by Map Index and show Map Index/ID in dropdown.
- https://github.com/Sebanisu/Field-Map-Editor/pull/204 - Major rendering bug. Not clearing texture slots when flushing vertice buffer.

### Notes
- Switching tp Github Actions will change the versioning. Because I used Appveyor to track the version number.

## [1.0.2011] - September 20, 2025

### Added
- TOML editor for Pupu Texture Combinations.
- BatchRenderer for improved rendering performance.
- ImGuizmo for interactive tile manipulation.
- Checkerboard shader for enhanced visual rendering.
- Filtertags for swizzle_one_image.
- ForceReloadingOfTextures to control texture loading explicitly.
- Edge scrolling when dragging a gizmo.
- Filter to field picker for improved usability.
- Ability to remove selected tiles with escape key.

### Changed
- Revamped texture path handling.
- Renamed Upscale to Swizzle for clarity.
- Replaced SFML with OpenGL-based glengine for rendering.
- Removed `sf::Texture`, `sf::RenderTexture`, and other SFML dependencies.
- Moved key_to_string to header and improved configuration saving.
- Updated texture page grid handling for MIM.
- Improved framebuffer handling to prevent black screens during field toggling.
- Optimized async path loading to reduce crashes and improve UI responsiveness.
- Restructured map rendering with `bind_color_attachment` and improved blending modes.
- Converted `sf::Vector` to `glm::vec` and `sf::IntRect` to `glm::ivec4`.
- Replaced checkerboard pattern with shader and framebuffer.
- Updated CMakeLists.txt to reflect SFML removal and experimental OpenGL support.
- Moved various functions (e.g., `set_uniform`) to appropriate files for better organization.
- Improved error handling with stack traces and better logging via spdlog.
- Adjusted gizmo scaling and fixed scroll bar interactions.
- Updated map history to prevent excessive updates during multi-frame operations.
- Fixed color button display bug by storing `tile_button_options` by value.
- Optimized saving of images with `save_image_pbo` and improved texture loading stability.

### Fixed
- Fixed OpenGL context closing early.
- Resolved crashes in async code by restructuring to avoid race conditions.
- Fixed texture page grid issues for MIM.
- Corrected blending mode issues with help from GPT.
- Fixed saving selected fields in the map list on batch screen.
- Addressed framebuffer cloning issues for RGB textures.
- Fixed bugs related to tile placement restrictions.
- Corrected color menu display to use two columns.
- Fixed GCC-specific compilation errors (e.g., missing headers, sign-conversion warnings).
- Resolved div-by-zero issue in batch processing.
- Fixed map order issues for specific fields.

### Removed
- Removed SFML dependencies, including `sf::Image` and `sf::ContextSettings`.
- Commented out unused `square.cpp` and related RenderTexture code.
- Removed `control_panel_window.cpp` due to tight coupling with `gui.cpp`.
- Disabled import window temporarily to focus on stability.

**Related Issues & PRs:** 
- [#157](https://github.com/Sebanisu/Field-Map-Editor/pull/157) - Revamp texture path handling, rename Upscale to Swizzle, and TOML editor for Pupu Texture Combinations.
- [#154](https://github.com/Sebanisu/Field-Map-Editor/issues/154) - BlendModeSettings fix.
- [#149](https://github.com/Sebanisu/Field-Map-Editor/pull/149) - Remove SFML.
- [#81](https://github.com/Sebanisu/Field-Map-Editor/issues/81) - BPP validity when changing x position.

## [1.0.1365] - July 8, 2025

### Added
- Custom path parsing with regex for FFNX config.
- Pupu ID support in custom paths window.
- Table-based UI for custom paths with tooltips.
- Selected path tracking for batch processing.
- `root_path_types` for flexible directory selection.
- `safe_copy_string` for robust string handling.
- FilterSettings with toggle save/load config.
- GenericComboClassWithFilterAndFixedToggles to disable invalid options dynamically.
- Explore button for input paths in batch processing.
- PatternSelector for updating configuration keys.
- Map picker functionality with directory browser support.
- Functions to copy maps from FF8 back to `maphistory`.
- `first_to_working_and_original` to restore .map files with undo support.
- `generate_(de)swizzle_map_paths` for map path handling.
- Sorting and duplicate removal for map paths.

### Changed
- Improved UI for saving settings in custom paths window.
- Moved variables to selections for better state management.
- Restructured texture loading to support non-path inputs.
- Optimized batch processing with new `key_value_data` class.
- Replaced `append_file_structure` with new path handling logic.
- Moved patterns to selections and added configuration saving.
- Switched to singletons for certain components.
- Updated `conanfile.py` to use CMake 3.31.6 and Visual Studio 17 2022 generator.
- Improved map sprite access to selections.
- Reformatted and cleaned up code for consistency (e.g., `fme::color`, `gui.cpp`).
- Enhanced logging with spdlog for filter messages and background color saves.
- Updated sorting functions for better performance.

### Fixed
- Fixed issue with imgui-sfml macro handling via upstream fork.
- Corrected double-saving of selections in batch processing.
- Fixed upscale/swizzle path parsing for manual additions.
- Resolved issues with map loading when filters are enabled.
- Fixed output path storage issues in the textbox.
- Addressed div-by-zero errors in batch processing.
- Fixed `batch_flatten_type` and `batch_compact_type` save/load issues.
- Corrected map order issues for specific fields.

### Removed
- Removed unneeded `load_configuration` function.
- Eliminated redundant `open_viii::LangT` coo from function calls.
- Commented out unused batch pattern tests.

**Related Issues & PRs:** 
- [#144](https://github.com/Sebanisu/Field-Map-Editor/pull/144) - Configure output paths need to diffs.

## [1.0.1129] - June 5, 2025

### Added
- Support for configuring output paths with diff handling.
- Added `ctre` to Conan for regex support.

### Changed
- Updated `.gitignore` to exclude unwanted files.

**Related Issues & PRs:** None found.

## [1.0.1056] - May 26, 2025

### Changed
- Updated `.gitignore` to exclude additional unwanted files.

**Related Issues & PRs:** None found.

---

**Note**: Version numbering started in 2025, with earlier builds using incremental build numbers. The changelog is constructed based on the provided GitHub tags and commit messages, with Canary including additional unreleased changes. Related issues and PRs are linked where explicitly mentioned in commit messages.
