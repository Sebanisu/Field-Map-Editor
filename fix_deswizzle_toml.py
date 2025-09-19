import toml
from pathlib import Path

# Path to your TOML file inside the res subdirectory
toml_file = Path("res") / "deswizzle.toml"

# Load the TOML
with open(toml_file, "r", encoding="utf-8") as f:
    data = toml.load(f)

# Track all existing filenames to avoid duplicates
existing_filenames = set()

def collect_filenames(obj):
    """Recursively collect all section names ending in .png."""
    if isinstance(obj, dict):
        for k, v in obj.items():
            if k.endswith(".png"):
                existing_filenames.add(k)
            collect_filenames(v)

collect_filenames(data)

def find_new_hex(pupu_ids):
    """Return first unused 8-digit hex from pupu_ids."""
    for pupu in pupu_ids:
        hex_id = f"{pupu:08X}"
        if hex_id not in existing_filenames:
            return hex_id
    return None

def process_sections(obj):
    """Recursively update timestamped section names."""
    if isinstance(obj, dict):
        to_update = []
        for k, v in obj.items():
            if "_202" in k and k.endswith(".png") and "filter_multi_pupu" in v:
                to_update.append((k, v))
            # Recurse into sub-dicts
            process_sections(v)
        
        for old_name, content in to_update:
            pupu_ids = content.get("filter_multi_pupu", [])
            prefix, _ = old_name.split("_202", 1)
            ext = old_name[old_name.rfind("."):]  # .png or other extension
            new_hex = find_new_hex(pupu_ids)
            if new_hex:
                new_name = f"{prefix}_{new_hex}{ext}"
                obj[new_name] = content
                del obj[old_name]
                existing_filenames.add(new_name)

# Process the whole TOML recursively
process_sections(data)

# Write back the updated TOML
with open(toml_file, "w", encoding="utf-8") as f:
    toml.dump(data, f)

print("TOML file updated successfully for all sections and languages.")