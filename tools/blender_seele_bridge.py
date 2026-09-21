#!/usr/bin/env python3
"""
twilight_elys/blender_seele_bridge.py
=====================================
Blender <-> Seele AI Bridge for Twilight Elysium Engine.

Runs Blender in headless/batch mode to generate 3D models,
textures, and materials under Seele AI control.

Usage:
  blender --background --python blender_seele_bridge.py -- [args]
  python3 blender_seele_bridge.py --export-models --output-dir /path
  python3 blender_seele_bridge.py --generate-material --type organic
"""

import bpy
import math
import os
import sys
import json
import random
import argparse

# ============================================================================
# CONFIGURATION
# ============================================================================

EXPORT_DIR = os.path.expanduser("~/.twilight-elys/blender_export")
MATERIAL_PRESETS = {
    "organic": {"roughness": 0.8, "metallic": 0.0, "specular": 0.3},
    "cyber": {"roughness": 0.2, "metallic": 0.9, "specular": 0.8},
    "mythic": {"roughness": 0.4, "metallic": 0.3, "specular": 0.6},
    "dystopic": {"roughness": 0.9, "metallic": 0.1, "specular": 0.1},
    "geometric": {"roughness": 0.5, "metallic": 0.5, "specular": 0.5},
}

# ============================================================================
# MODEL GENERATORS
# ============================================================================

class BlenderModelGen:
    """Procedural model generation using Blender's mesh tools."""
    
    def __init__(self):
        self.generated = []
        self._cleanup_scene()
    
    def _cleanup_scene(self):
        """Remove default cube, camera, light."""
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)
    
    def _register_object(self, obj):
        """Register object by name (safe across join operations)."""
        self.generated.append(obj.name)
    
    def _apply_material(self, obj, preset_name="organic", color=None):
        """Apply a PBR material preset."""
        preset = MATERIAL_PRESETS.get(preset_name, MATERIAL_PRESETS["organic"])
        mat = bpy.data.materials.new(name=f"seele_{preset_name}")
        mat.use_nodes = True
        bsdf = mat.node_tree.nodes["Principled BSDF"]
        bsdf.inputs["Roughness"].default_value = preset["roughness"]
        bsdf.inputs["Metallic"].default_value = preset["metallic"]
        bsdf.inputs["Specular IOR Level"].default_value = preset["specular"]
        if color:
            bsdf.inputs["Base Color"].default_value = (*color, 1.0)
        obj.data.materials.append(mat)
    
    def generate_tree(self, tree_type="oak", height=3.0):
        """Generate a procedural tree with trunk + canopy."""
        existing = set(bpy.data.objects.keys())
        
        bpy.ops.mesh.primitive_cylinder_add(
            radius=0.1, depth=height * 0.4, location=(0, 0, height * 0.2)
        )
        trunk = bpy.context.active_object
        trunk.name = f"tree_{tree_type}_trunk"
        
        # Canopy based on type
        canopy_r = height * 0.3
        if tree_type == "bamboo":
            canopy_r = height * 0.05
            canopy_h = height * 0.6
            bpy.ops.mesh.primitive_cylinder_add(
                radius=canopy_r, depth=canopy_h,
                location=(0, 0, height * 0.4 + canopy_h / 2)
            )
        elif tree_type in ("pine", "cedar"):
            bpy.ops.mesh.primitive_cone_add(
                radius1=canopy_r, radius2=0, depth=height * 0.7,
                location=(0, 0, height * 0.4 + height * 0.35)
            )
        else:
            bpy.ops.mesh.primitive_ico_sphere_add(
                radius=canopy_r, location=(0, 0, height * 0.6)
            )
        
        canopy = bpy.context.active_object
        canopy.name = f"tree_{tree_type}_canopy"
        
        # Apply materials
        self._apply_material(trunk, "organic", (0.4, 0.25, 0.1))
        self._apply_material(canopy, "organic", (0.1, 0.5, 0.1))
        
        # Select only new objects and join
        new_objects = [bpy.data.objects[name] for name in bpy.data.objects.keys() if name not in existing]
        bpy.ops.object.select_all(action='DESELECT')
        for obj in new_objects:
            obj.select_set(True)
        bpy.context.view_layer.objects.active = trunk
        bpy.ops.object.join()
        
        obj = bpy.context.active_object
        obj.name = f"tree_{tree_type}"
        self._register_object(obj)
        return obj
    
    def generate_character(self, character_type="kite", height=1.8):
        """Generate a humanoid character mesh."""
        existing = set(bpy.data.objects.keys())
        
        # Body
        bpy.ops.mesh.primitive_cube_add(
            size=1, location=(0, 0, height * 0.5)
        )
        body = bpy.context.active_object
        body.name = f"{character_type}_body"
        body.scale = (0.3, 0.2, height * 0.4)
        bpy.ops.object.transform_apply(scale=True)
        
        # Head
        bpy.ops.mesh.primitive_uv_sphere_add(
            radius=height * 0.08, location=(0, 0, height * 0.85)
        )
        head = bpy.context.active_object
        head.name = f"{character_type}_head"
        
        # Arms
        for side in [-1, 1]:
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(side * height * 0.2, 0, height * 0.5)
            )
            arm = bpy.context.active_object
            arm.name = f"{character_type}_arm_{'L' if side < 0 else 'R'}"
            arm.scale = (0.05, 0.05, height * 0.3)
            bpy.ops.object.transform_apply(scale=True)
        
        # Legs
        for side in [-1, 1]:
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(side * height * 0.08, 0, height * 0.2)
            )
            leg = bpy.context.active_object
            leg.name = f"{character_type}_leg_{'L' if side < 0 else 'R'}"
            leg.scale = (0.07, 0.07, height * 0.2)
            bpy.ops.object.transform_apply(scale=True)
        
        # Select only newly created objects and join
        new_objects = [bpy.data.objects[name] for name in bpy.data.objects.keys() if name not in existing]
        bpy.ops.object.select_all(action='DESELECT')
        for obj in new_objects:
            obj.select_set(True)
        bpy.context.view_layer.objects.active = body
        bpy.ops.object.join()
        
        obj = bpy.context.active_object
        obj.name = f"char_{character_type}"
        
        # Color scheme by character
        colors = {
            "kite": (0.2, 0.4, 0.9),
            "haseo": (0.1, 0.1, 0.1),
            "blackrose": (0.1, 0.6, 0.2),
            "balung": (0.8, 0.8, 0.9),
        }
        self._apply_material(obj, "organic", colors.get(character_type, (0.5, 0.5, 0.5)))
        
        self._register_object(obj)
        return obj
    
    def generate_monster(self, monster_type="skeleton", height=1.7):
        """Generate a monster mesh."""
        existing = set(bpy.data.objects.keys())
        
        if monster_type == "data_bug":
            bpy.ops.mesh.primitive_ico_sphere_add(radius=height * 0.15, location=(0, 0, height * 0.15))
        elif monster_type == "crystal":
            bpy.ops.mesh.primitive_ico_sphere_add(radius=height * 0.4, location=(0, 0, height * 0.4))
        else:
            # Humanoid monsters
            bpy.ops.mesh.primitive_cube_add(size=1, location=(0, 0, height * 0.5))
            body = bpy.context.active_object
            body.name = f"monster_{monster_type}_body"
            body.scale = (0.35, 0.25, height * 0.45)
            bpy.ops.object.transform_apply(scale=True)
            
            bpy.ops.mesh.primitive_uv_sphere_add(
                radius=height * 0.1, location=(0, 0, height * 0.9)
            )
            head = bpy.context.active_object
            head.name = f"monster_{monster_type}_head"
            
            # Select only new objects and join
            new_objects = [bpy.data.objects[name] for name in bpy.data.objects.keys() if name not in existing]
            bpy.ops.object.select_all(action='DESELECT')
            for obj in new_objects:
                obj.select_set(True)
            bpy.context.view_layer.objects.active = body
            bpy.ops.object.join()
        
        obj = bpy.context.active_object
        obj.name = f"monster_{monster_type}"
        
        monster_colors = {
            "skeleton": (0.9, 0.9, 0.85),
            "corrupted": (0.3, 0.1, 0.4),
            "golem": (0.4, 0.3, 0.2),
            "shadow": (0.05, 0.05, 0.1),
            "boss": (0.8, 0.1, 0.1),
            "dragon": (0.2, 0.5, 0.1),
        }
        self._apply_material(obj, "dystopic", monster_colors.get(monster_type, (0.5, 0.5, 0.5)))
        
        self._register_object(obj)
        return obj
    
    def generate_weapon(self, weapon_type="sword", length=1.0):
        """Generate a weapon mesh."""
        existing = set(bpy.data.objects.keys())
        
        if weapon_type == "sword":
            # Blade
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, length * 0.5)
            )
            blade = bpy.context.active_object
            blade.name = f"weapon_{weapon_type}_blade"
            blade.scale = (0.03, 0.01, length * 0.4)
            bpy.ops.object.transform_apply(scale=True)
            
            # Hilt
            bpy.ops.mesh.primitive_cylinder_add(
                radius=0.02, depth=length * 0.15,
                location=(0, 0, length * 0.08)
            )
            hilt = bpy.context.active_object
            hilt.name = f"weapon_{weapon_type}_hilt"
            
            # Select only new objects and join
            new_objects = [bpy.data.objects[name] for name in bpy.data.objects.keys() if name not in existing]
            bpy.ops.object.select_all(action='DESELECT')
            for obj in new_objects:
                obj.select_set(True)
            bpy.context.view_layer.objects.active = blade
            bpy.ops.object.join()
        elif weapon_type == "staff":
            bpy.ops.mesh.primitive_cylinder_add(
                radius=0.03, depth=length, location=(0, 0, length * 0.5)
            )
        elif weapon_type == "greatsword":
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, length * 0.5)
            )
            blade = bpy.context.active_object
            blade.name = f"weapon_{weapon_type}_blade"
            blade.scale = (0.06, 0.015, length * 0.5)
            bpy.ops.object.transform_apply(scale=True)
        else:
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, length * 0.5)
            )
        
        obj = bpy.context.active_object
        obj.name = f"weapon_{weapon_type}"
        self._apply_material(obj, "cyber", (0.7, 0.7, 0.8))
        self._register_object(obj)
        return obj
    
    def generate_structure(self, struct_type="tower", height=5.0):
        """Generate architectural structures."""
        existing = set(bpy.data.objects.keys())

        if struct_type == "tower":
            bpy.ops.mesh.primitive_cylinder_add(
                radius=height * 0.1, depth=height, vertices=8,
                location=(0, 0, height * 0.5)
            )
        elif struct_type == "pillar":
            bpy.ops.mesh.primitive_cylinder_add(
                radius=height * 0.05, depth=height, vertices=12,
                location=(0, 0, height * 0.5)
            )
        elif struct_type == "gate":
            # Two pillars + lintel
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(-height * 0.3, 0, height * 0.5)
            )
            left = bpy.context.active_object
            left.name = "gate_left"
            left.scale = (0.1, 0.2, height * 0.5)
            bpy.ops.object.transform_apply(scale=True)
            
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(height * 0.3, 0, height * 0.5)
            )
            right = bpy.context.active_object
            right.name = "gate_right"
            right.scale = (0.1, 0.2, height * 0.5)
            bpy.ops.object.transform_apply(scale=True)
            
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, height * 0.9)
            )
            lintel = bpy.context.active_object
            lintel.name = "gate_lintel"
            lintel.scale = (height * 0.35, 0.2, 0.1)
            bpy.ops.object.transform_apply(scale=True)
            
            # Select only new objects and join
            new_objects = [bpy.data.objects[name] for name in bpy.data.objects.keys() if name not in existing]
            bpy.ops.object.select_all(action='DESELECT')
            for obj in new_objects:
                obj.select_set(True)
            bpy.context.view_layer.objects.active = left
            bpy.ops.object.join()

        elif struct_type == "crystal":
            bpy.ops.mesh.primitive_ico_sphere_add(
                radius=height * 0.3, location=(0, 0, height * 0.3)
            )
        elif struct_type == "monolith":
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, height * 0.5)
            )
            obj = bpy.context.active_object
            obj.name = "monolith"
            obj.scale = (height * 0.2, height * 0.05, height * 0.5)
            bpy.ops.object.transform_apply(scale=True)
        else:
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, height * 0.5)
            )
        
        obj = bpy.context.active_object
        obj.name = f"struct_{struct_type}"
        self._apply_material(obj, "mythic", (0.5, 0.4, 0.3))
        self._register_object(obj)
        return obj
    
    def generate_item(self, item_type="potion", size=0.3):
        """Generate item meshes."""
        if item_type in ("potion", "ether", "revive"):
            bpy.ops.mesh.primitive_cylinder_add(
                radius=size * 0.3, depth=size, location=(0, 0, size * 0.5)
            )
        elif item_type == "crystal":
            bpy.ops.mesh.primitive_ico_sphere_add(
                radius=size * 0.4, location=(0, 0, size * 0.4)
            )
        elif item_type == "key":
            bpy.ops.mesh.primitive_cube_add(
                size=1, location=(0, 0, size * 0.5)
            )
            obj = bpy.context.active_object
            obj.name = f"item_{item_type}"
            obj.scale = (size * 0.3, size, size * 0.1)
            bpy.ops.object.transform_apply(scale=True)
        else:
            bpy.ops.mesh.primitive_cube_add(
                size=size, location=(0, 0, size * 0.5)
            )
        
        obj = bpy.context.active_object
        obj.name = f"item_{item_type}"
        self._apply_material(obj, "organic", (0.8, 0.6, 0.2))
        self._register_object(obj)
        return obj
    
    def generate_sky_grid(self, size=200):
        """Generate a grid plane for skybox."""
        bpy.ops.mesh.primitive_plane_add(size=size, location=(0, 0, 0))
        obj = bpy.context.active_object
        obj.name = "sky_grid"
        # Subdivide for grid effect
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.subdivide(number_cuts=20)
        bpy.ops.object.mode_set(mode='OBJECT')
        self._apply_material(obj, "mythic", (0.1, 0.1, 0.2))
        self._register_object(obj)
        return obj
    
    def export_all(self, output_dir, format="glTF"):
        """Export all generated models."""
        os.makedirs(output_dir, exist_ok=True)
        exported = []
        
        # Capture names first (object references may become invalid after join)
        model_names = list(self.generated)  # Already strings
        
        for name in model_names:
            obj = bpy.data.objects.get(name)
            if not obj:
                continue
            
            filepath = os.path.join(output_dir, name)
            
            # Select only this object
            bpy.ops.object.select_all(action='DESELECT')
            obj.select_set(True)
            bpy.context.view_layer.objects.active = obj
            
            full_path = filepath + ".glb"
            try:
                bpy.ops.export_scene.gltf(
                    filepath=full_path,
                    use_selection=True,
                    export_format='GLB',
                    export_materials='EXPORT',
                )
            except Exception as e:
                print(f"glTF export failed for {name}: {e}")
                full_path = filepath + ".blend"
                bpy.ops.wm.save_as_mainfile(filepath=full_path)
            
            exported.append({
                "name": name,
                "path": full_path,
                "format": format,
                "vertices": len(obj.data.vertices),
                "faces": len(obj.data.polygons),
            })
        
        # Write manifest
        manifest_path = os.path.join(output_dir, "seele_manifest.json")
        with open(manifest_path, 'w') as f:
            json.dump({"models": exported, "count": len(exported)}, f, indent=2)
        
        return exported


# ============================================================================
# MATERIAL TEXTURE GENERATION
# ============================================================================

class TextureGenerator:
    """Generate PBR texture sets using Blender's procedural textures."""
    
    def __init__(self, output_dir):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
    
    def generate_pbr_set(self, name, width=1024, height=1024, style="organic"):
        """Generate albedo, normal, roughness, metallic, AO maps."""
        textures = {}
        
        # Create a temporary plane for baking
        bpy.ops.mesh.primitive_plane_add(size=2)
        obj = bpy.context.active_object
        obj.name = f"tex_bake_{name}"
        
        # Unwrap
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.uv.smart_project()
        bpy.ops.object.mode_set(mode='OBJECT')
        
        # Create image for each map
        for map_type in ["albedo", "normal", "roughness", "metallic", "ao"]:
            img = bpy.data.images.new(
                f"{name}_{map_type}", width=width, height=height
            )
            img.filepath_raw = os.path.join(
                self.output_dir, f"{name}_{map_type}.png"
            )
            img.file_format = 'PNG'
            
            # Create material with procedural texture
            mat = bpy.data.materials.new(name=f"tex_mat_{name}_{map_type}")
            mat.use_nodes = True
            nodes = mat.node_tree.nodes
            links = mat.node_tree.links
            
            # Clear default nodes
            nodes.clear()
            
            # Add output
            output = nodes.new('ShaderNodeOutputMaterial')
            output.location = (400, 0)
            
            # Add principled BSDF
            bsdf = nodes.new('ShaderNodeBsdfPrincipled')
            bsdf.location = (0, 0)
            
            if map_type == "albedo":
                # Color texture based on style
                tex_coord = nodes.new('ShaderNodeTexCoord')
                tex_coord.location = (-800, 0)
                noise = nodes.new('ShaderNodeTexNoise')
                noise.location = (-400, 0)
                noise.inputs['Scale'].default_value = 5.0 if style == "organic" else 2.0
                links.new(tex_coord.outputs['Object'], noise.inputs['Vector'])
                # Add some color variation
                color_ramp = nodes.new('ShaderNodeValToRGB')
                color_ramp.location = (-200, 0)
                if style == "organic":
                    color_ramp.color_ramp.elements[0].color = (0.1, 0.4, 0.05, 1)
                    color_ramp.color_ramp.elements[1].color = (0.3, 0.6, 0.1, 1)
                elif style == "cyber":
                    color_ramp.color_ramp.elements[0].color = (0.0, 0.05, 0.1, 1)
                    color_ramp.color_ramp.elements[1].color = (0.0, 0.3, 0.8, 1)
                else:
                    color_ramp.color_ramp.elements[0].color = (0.3, 0.3, 0.3, 1)
                    color_ramp.color_ramp.elements[1].color = (0.7, 0.7, 0.7, 1)
                links.new(noise.outputs['Fac'], color_ramp.inputs['Fac'])
                links.new(color_ramp.outputs['Color'], bsdf.inputs['Base Color'])
                
            elif map_type == "roughness":
                value = MATERIAL_PRESETS.get(style, MATERIAL_PRESETS["organic"])["roughness"]
                bsdf.inputs['Roughness'].default_value = value
                
            elif map_type == "metallic":
                value = MATERIAL_PRESETS.get(style, MATERIAL_PRESETS["organic"])["metallic"]
                bsdf.inputs['Metallic'].default_value = value
            
            links.new(bsdf.outputs['BSDF'], output.inputs['Surface'])
            
            obj.data.materials.append(mat)
            
            # Save image
            img.save()
            textures[map_type] = img.filepath_raw
        
        # Cleanup
        bpy.data.objects.remove(obj, do_unlink=True)
        
        return textures


# ============================================================================
# MAIN ENTRY POINT
# ============================================================================

def main():
    parser = argparse.ArgumentParser(description="Blender-Seele Bridge")
    parser.add_argument("--export-models", action="store_true", help="Export all models")
    parser.add_argument("--output-dir", default=EXPORT_DIR, help="Output directory")
    parser.add_argument("--format", default="glTF", choices=["glTF", "OBJ", "FBX"])
    parser.add_argument("--model-type", default="all", help="Model type to generate")
    parser.add_argument("--generate-material", action="store_true", help="Generate materials")
    parser.add_argument("--material-style", default="organic", help="Material style")
    parser.add_argument("--texture-size", type=int, default=512, help="Texture resolution")
    parser.add_argument("--seed", type=int, default=None, help="Random seed")
    
    # Parse args after '--'
    args = parser.parse_args(sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else [])
    
    if args.seed is not None:
        random.seed(args.seed)
    
    gen = BlenderModelGen()
    
    # Generate models based on type
    if args.model_type in ("all", "tree"):
        for t in ["oak", "pine", "willow", "bamboo", "cedar"]:
            gen.generate_tree(t)
    
    if args.model_type in ("all", "character"):
        for c in ["kite", "haseo", "blackrose", "balung"]:
            gen.generate_character(c)
    
    if args.model_type in ("all", "monster"):
        for m in ["skeleton", "corrupted", "golem", "shadow", "boss"]:
            gen.generate_monster(m)
    
    if args.model_type in ("all", "weapon"):
        for w in ["sword", "greatsword", "staff", "spear", "dagger"]:
            gen.generate_weapon(w, random.uniform(0.5, 2.0))
    
    if args.model_type in ("all", "structure"):
        for s in ["tower", "pillar", "gate", "crystal", "monolith"]:
            gen.generate_structure(s, random.uniform(2.0, 8.0))
    
    if args.model_type in ("all", "item"):
        for i in ["potion", "crystal", "key"]:
            gen.generate_item(i)
    
    if args.model_type in ("all", "sky"):
        gen.generate_sky_grid()
    
    # Generate materials if requested
    if args.generate_material:
        tex_gen = TextureGenerator(os.path.join(args.output_dir, "textures"))
        for style in ["organic", "cyber", "mythic", "dystopic"]:
            tex_gen.generate_pbr_set(f"seele_{style}", args.texture_size, args.texture_size, style)
    
    # Export
    if args.export_models or True:  # Always export
        exported = gen.export_all(args.output_dir, args.format)
        print(f"Exported {len(exported)} models to {args.output_dir}")
        for e in exported:
            print(f"  - {e['name']}: {e['vertices']} verts, {e['faces']} faces")

if __name__ == "__main__":
    main()
