#!/usr/bin/env python3
"""
Twilight Elysium — Unified Worldbuild Manifest Scanner
========================================================
Scans all remake project Content/ and assets/ directories plus the
Bethesda game installs (Skyrim SE, Oblivion) and produces a single
JSON manifest describing every texture, model, script, map, audio,
and data asset available for worldbuild generation.

Output: /home/sin/Projects/twilight-elysium/assets/worldbuild_manifest.json
Also writes a human-readable index: ASSET_INDEX.md (replace).

Usage:
    python3 scan_worldbuilds.py [--verify]

--verify  : also check that every referenced file still exists on disk
            and report missing entries (stale references).
"""

from __future__ import annotations

import argparse
import datetime
import json
import os
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

# ── Project roots ──────────────────────────────────────────────────────────

ROOT = Path("/home/sin/Projects")
STEAM = Path("/home/sin/.local/share/Steam/steamapps/common")

PROJECTS = {
    "dot-hack-remake":     ROOT / "dot-hack-remake",
    "black-ops-2-elysium": ROOT / "black-ops-2-elysium",
    "tf2-elysium":         ROOT / "tf2-elysium",
    "half-life-elysium":   ROOT / "half-life-elysium",
    "half-life-2-elysium": ROOT / "half-life-2-elysium",
    "kingdom-hearts-zero": ROOT / "kingdom-hearts-zero",
    "te-bonfire":          ROOT / "te-bonfire",
    "ringworld-redux":     ROOT / "ringworld-redux",
    "twilight-elysium":    ROOT / "twilight-elysium",
    "gaming-unified":      ROOT / "gaming-unified",
    "living-sin-assets":   ROOT / "living-sin-assets",
    "firedragon":          ROOT / "firedragon",
}

GAMES = {
    "skyrim-special-edition": STEAM / "Skyrim Special Edition",
    "oblivion":                STEAM / "Oblivion",
}

# ── Asset type classifiers ────────────────────────────────────────────────

EXT_MAP: Dict[str, List[str]] = {
    "texture":  [".png", ".jpg", ".jpeg", ".dds", ".tga", ".bmp", ".exr", ".hdr"],
    "model":    [".nif", ".obj", ".fbx", ".gltf", ".glb", ".3ds", ".stl", ".ply", ".blend"],
    "shader":   [".frag", ".vert", ".glsl", ".hlsl", ".shader", ".spv", ".dxil"],
    "audio":    [".wav", ".mp3", ".ogg", ".flac", ".aiff", ".wma"],
    "script":   [".psc", ".pex", ".esp", ".esm", ".bsa", ".json", ".xml", ".toml", ".ini"],
    "data":     [".json", ".csv", ".bin", ".dat", ".map"],
    "video":    [".mp4", ".avi", ".mkv", ".webm"],
}

TYPE_ORDER = ["texture", "model", "shader", "audio", "script", "data", "video"]


def classify(ext: str) -> Optional[str]:
    ext = ext.lower()
    for typ, exts in EXT_MAP.items():
        if ext in exts:
            return typ
    return None


# ── Scanner ────────────────────────────────────────────────────────────────

def scan_recursive(root: Path, prefix: str = "") -> List[Dict[str, Any]]:
    """Recursively list all asset files under *root* with type metadata."""
    assets: List[Dict[str, Any]] = []
    if not root.is_dir():
        return assets
    for entry in sorted(root.rglob("*")):
        if not entry.is_file():
            continue
        # Skip enormous non-asset dirs
        rel = entry.relative_to(root)
        if any(part.startswith("node_modules") for part in rel.parts):
            continue
        if any(part.startswith(".venv") for part in rel.parts):
            continue
        if any(part.startswith("__pycache__") for part in rel.parts):
            continue
        if any(part.startswith(".git") for part in rel.parts):
            continue
        # Skip obj files that are clearly compiler temp artifacts
        if entry.name.startswith("switch_") and entry.suffix == ".obj":
            continue

        ext = entry.suffix
        typ = classify(ext)
        if typ is None:
            continue

        stat = entry.stat()
        assets.append({
            "path": str(entry),
            "relative": str(rel),
            "type": typ,
            "size_bytes": stat.st_size,
            "modified": datetime.datetime.fromtimestamp(
                stat.st_mtime, tz=datetime.timezone.utc
            ).isoformat(),
        })
    return assets


def scan_project(name: str, root: Path) -> Dict[str, Any]:
    """Scan one project's Content/ and assets/ directories."""
    result: Dict[str, Any] = {
        "project": name,
        "root": str(root),
        "content_dir": None,
        "assets_dir": None,
        "assets": [],
    }

    content = root / "Content"
    assets_dir = root / "assets"

    if content.is_dir():
        result["content_dir"] = str(content)
        result["assets"] = scan_recursive(content)
    if assets_dir.is_dir():
        result["assets_dir"] = str(assets_dir)
        result["assets"].extend(scan_recursive(assets_dir))

    # type breakdown
    type_counts: Dict[str, int] = {}
    for a in result["assets"]:
        t = a["type"]
        type_counts[t] = type_counts.get(t, 0) + 1
    result["type_counts"] = type_counts
    result["total_assets"] = len(result["assets"])
    return result


def scan_game(name: str, root: Path) -> Dict[str, Any]:
    """Scan a Bethesda game install for extractable assets."""
    result: Dict[str, Any] = {
        "game": name,
        "root": str(root),
        "assets": [],
        "type_counts": {},
        "total_assets": 0,
    }
    if not root.is_dir():
        result["error"] = f"Not installed: {root}"
        return result

    assets = scan_recursive(root)
    result["assets"] = assets
    type_counts: Dict[str, int] = {}
    for a in assets:
        t = a["type"]
        type_counts[t] = type_counts.get(t, 0) + 1
    result["type_counts"] = type_counts
    result["total_assets"] = len(assets)
    return result


# ── Verification ──────────────────────────────────────────────────────────

def verify_manifest(manifest: Dict[str, Any]) -> Dict[str, Any]:
    """Check that every file referenced in the manifest still exists."""
    missing: List[str] = []
    stale_projects: List[str] = []
    for section in manifest.get("projects", []):
        for asset in section.get("assets", []):
            if not Path(asset["path"]).exists():
                missing.append(asset["path"])
        if missing:
            stale_projects.append(section["project"])
    for section in manifest.get("games", []):
        for asset in section.get("assets", []):
            if not Path(asset["path"]).exists():
                missing.append(asset["path"])
        if missing:
            stale_projects.append(section["game"])
    return {
        "missing_count": len(missing),
        "missing": missing[:100],  # first 100
        "stale_projects": stale_projects,
    }


# ── Main ───────────────────────────────────────────────────────────────────

def main() -> int:
    parser = argparse.ArgumentParser(
        description="Scan all remake project assets into a unified worldbuild manifest."
    )
    parser.add_argument(
        "--verify",
        action="store_true",
        help="Verify referenced files exist; report missing entries.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("/home/sin/Projects/twilight-elysium/assets/worldbuild_manifest.json"),
        help="Output JSON path (default: assets/worldbuild_manifest.json).",
    )
    args = parser.parse_args()

    # Ensure output directory
    args.output.parent.mkdir(parents=True, exist_ok=True)

    manifest: Dict[str, Any] = {
        "generated_at": datetime.datetime.now(tz=datetime.timezone.utc).isoformat(),
        "schema_version": "1.0",
        "description": (
            "Unified worldbuild asset manifest for Twilight Elysium engine. "
            "Aggregates all textures, models, shaders, audio, scripts, and data "
            "from every remake project + Bethesda source games into one index "
            "so the engine can generate and assemble final alpha testing worldbuilds."
        ),
        "projects": [],
        "games": [],
        "summary": {},
    }

    # ── Projects ──
    for name, root in sorted(PROJECTS.items()):
        print(f"[scan] project: {name} ... ", end="", flush=True)
        proj = scan_project(name, root)
        manifest["projects"].append(proj)
        print(
            f"{proj['total_assets']} assets "
            f"({', '.join(f'{k}:{v}' for k, v in sorted(proj['type_counts'].items()))})"
        )

    # ── Games ──
    for name, root in sorted(GAMES.items()):
        print(f"[scan] game: {name} ... ", end="", flush=True)
        game = scan_game(name, root)
        manifest["games"].append(game)
        if "error" in game:
            print(f"SKIP — {game['error']}")
        else:
            print(
                f"{game['total_assets']} assets "
                f"({', '.join(f'{k}:{v}' for k, v in sorted(game['type_counts'].items()))})"
            )

    # ── Summary ──
    total_assets = sum(p["total_assets"] for p in manifest["projects"]) + sum(
        g["total_assets"] for g in manifest["games"]
    )
    type_totals: Dict[str, int] = {}
    for p in manifest["projects"]:
        for t, c in p["type_counts"].items():
            type_totals[t] = type_totals.get(t, 0) + c
    for g in manifest["games"]:
        for t, c in g["type_counts"].items():
            type_totals[t] = type_totals.get(t, 0) + c

    manifest["summary"] = {
        "total_projects": len(manifest["projects"]),
        "total_games_scanned": len(manifest["games"]),
        "total_assets": total_assets,
        "type_breakdown": dict(sorted(type_totals.items(), key=lambda x: -x[1])),
    }

    # ── Write ──
    args.output.write_text(json.dumps(manifest, indent=2))
    print(f"\n[write] manifest -> {args.output}")
    print(f"[summary] {total_assets} total assets across "
          f"{len(manifest['projects'])} projects + {len(manifest['games'])} games")

    # ── Optional verification ──
    if args.verify:
        print("[verify] checking referenced files on disk ...")
        v = verify_manifest(manifest)
        print(f"[verify] missing={v['missing_count']}")
        if v["stale_projects"]:
            print(f"[verify] stale projects: {', '.join(sorted(v['stale_projects']))}")
        if v["missing"]:
            print("[verify] first missing paths:")
            for p in v["missing"][:20]:
                print(f"  MISSING: {p}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
