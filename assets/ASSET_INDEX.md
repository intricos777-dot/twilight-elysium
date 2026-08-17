# Asset Index — Unified Worldbuild Manifest

Generated: 2026-08-17T01:45:00+00:00  
Manifest: [worldbuild_manifest.json](worldbuild_manifest.json)  
Schema: `{"generated_at","schema_version","projects","games","summary"}`

## Summary

| Category | Count |
|---|---|
| Projects scanned | 12 |
| Games scanned | 2 (Skyrim SE, Oblivion) |
| **Total assets** | **2,121** |

### Type breakdown

| Type | Assets |
|---|---|
| texture | 1,734 |
| script | 246 |
| audio | 126 |
| model | 2 |
| shader | 12 |
| data | 1 |

## Projects

### dot-hack-remake — 1,842 assets
- **textures:** 1,728 (seele/cmn_a, seele/vol1_i — NPC sprites, environment maps, UI textures)
- **audio:** 80 (voice lines: haseo, kite, black_rose, shino, shugo, tsukasa, rose)
- **scripts:** 31 (world definitions, canon timeline, character data, save files, mod rules)
- **shaders:** 3 (VR commons shader)

Content roots:
- `Content/Assets/seele/cmn_a/` — common assets (base + HD), vol1_i disc textures
- `Content/Assets/seele/vol1_i/` — disc 1 textures (base, hd, restyled, blended)
- `Content/Audio/Voices/` — character voice WAV lines
- `Content/Worlds/` — zone + world JSON definitions
- `Content/Canon/` — timeline, arcs (JSON)
- `Content/Data/` — campaign order, world settings

### black-ops-2-elysium — 12 assets
- **scripts:** 12 (weapon/perk/enemy/map JSON data for DS1 + BO2)

### tf2-elysium — 12 assets
- **scripts:** 12 (class/weapon/map/game mode JSON data, 2 character model defs)

### half-life-elysium — 8 assets
- **scripts:** 8 (HL1 weapon/enemy/map JSON data)

### half-life-2-elysium — 8 assets
- **scripts:** 8 (HL2 weapon/enemy/map JSON data)

### kingdom-hearts-zero — 36 assets
- **audio:** 27 (battle tracks, world drifts, keyblade SFX, boss variants)
- **shaders:** 8 (PS2-style frag/vert pairs: kh1, kh2, tron grid, UI)
- **scripts:** 1 (credits JSON)

### ringworld-redux — 22 assets
- **audio:** 19 (dark vampire / demon visualizer tracks, synth/rock tracks)
- **scripts:** 2 (character bios, campaign characters)
- **shaders:** 1 (basic frag)

### living-sin-assets — 8 assets
- **textures:** 8 (emblems, pointer/cursor PNGs + SVG)

### firedragon — 1 asset
- **scripts:** 1 (twilight theme manifest)

### te-bonfire — 0 assets (placeholder, manifest schema only)
### gaming-unified — 0 assets (aggregator only)
### twilight-elysium — 0 assets (engine repo, assets scanned from projects above)

## Bethesda Source Games

### Skyrim Special Edition — 155 assets
- **scripts:** 147 (68 Papyrus source `.psc` + 40 compiled `.pex` + 5 ESPs + 3 toml + 2 ini)
- **textures:** 8 (PNG UI elements)
- Source scripts in `Data/Scripts/Source/*.psc` (Actor, Quest, Spell, Shout, Flora, TreeObject, etc.)
- Compiled scripts in `Data/Scripts/*.pex`
- Mod ESPs: BlackMarsh, BoatTravel, Elsewhyr, NarrativeEngine, RealisticAIOverhaul

### Oblivion — 17 assets
- **textures:** 6 (DDS: 0wine01, label01, + normals)
- **models:** 2 (MakeDrinkBottle.nif, MakeDrinkRetort.nif)
- **scripts:** 9 (6 ESPs, 2 ini, 2 OBSE plugins)
  - ESPs: RadiantAI NPCs Alive, Apachii Wigs, DMRA Robe Replacer, ORM Cloaks, Dream and Anime Eyes, HentaiChinaDress
  - Animation: ArmsCrossedStillMale.kf, StandHandsBack.kf, specialidle *.kf

## How it's used

The Twilight Elysium engine reads `assets/worldbuild_manifest.json` at build
configuration time to:

1. Discover all available textures for world builds (terrain, NPC, environment).
2. Load audio for each remake's soundtrack and voice set.
3. Import script/data JSONs as world rules (difficulty, map layout, NPC tables).
4. Cross-reference Bethesda source game assets (Skyrim SE / Oblivion) as
   rippable world-build source material for terrain, flora, tree, and blueprint
   extraction into TE world stages.

For generation commands, see `tools/scan_worldbuilds.py --help` and the
engine's worldbuild pipeline in `src/game/`.
