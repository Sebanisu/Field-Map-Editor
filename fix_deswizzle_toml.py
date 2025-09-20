from pathlib import Path
import tomlkit
from tomlkit import array

# Path to your TOML++ file
toml_file = Path("res") / "deswizzle.toml"

# Load TOML++ preserving formatting
with open(toml_file, "r", encoding="utf-8") as f:
    data = tomlkit.parse(f.read())

# Track all existing filenames to avoid duplicates
existing_filenames = set()
processed_count = 0  # Counter for sections updated
skipped_count = 0    # Counter for sections skipped

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

def process_sections(obj, depth=0):
    """Recursively update timestamped section names, make arrays multiline, and add old name comment."""
    global processed_count, skipped_count
    if isinstance(obj, dict):
        to_update = []
        for k, v in obj.items():
            if "_202" in k and k.endswith(".png") and "filter_multi_pupu" in v:
                to_update.append((k, v))
            else:
                skipped_count += 1
                if skipped_count % 50 == 0:
                    print(f"[Progress] Skipped {skipped_count} sections... Depth {depth}")
            process_sections(v, depth + 1)

        for old_name, content in to_update:
            # Convert filter_multi_pupu array to multiline
            if "filter_multi_pupu" in content:
                arr = array(content["filter_multi_pupu"])
                arr.multiline(True)
                content["filter_multi_pupu"] = arr

            pupu_ids = content.get("filter_multi_pupu", [])
            prefix, _ = old_name.split("_202", 1)
            ext = old_name[old_name.rfind("."):]  # e.g., .png
            new_hex = find_new_hex(pupu_ids)
            if new_hex:
                new_name = f"{prefix}_{new_hex}{ext}"
                # Add old name as a comment for Git visibility
                content["_renamed_from"] = old_name
                obj[new_name] = content
                del obj[old_name]
                existing_filenames.add(new_name)
            
            processed_count += 1
            if processed_count % 50 == 0:
                print(f"[Progress] Processed {processed_count} sections... Depth {depth}")

# Process the whole TOML recursively
process_sections(data)

# Write back the updated TOML++ file
with open(toml_file, "w", encoding="utf-8") as f:
    f.write(tomlkit.dumps(data))

print(f"TOML++ update complete. Total processed sections: {processed_count}, skipped: {skipped_count}")
