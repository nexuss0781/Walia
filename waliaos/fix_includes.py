import os

# Define the root of the source tree
SRC_DIR = "../src"

# Dynamically detect subdirectories and root headers in src/
SUBDIRS = [d for d in os.listdir(SRC_DIR) if os.path.isdir(os.path.join(SRC_DIR, d)) and not d.startswith(".")]
ROOT_HEADERS = [f for f in os.listdir(SRC_DIR) if f.endswith(".h")]

print(f">> Walia: Detected Subdirs: {SUBDIRS}")
print(f">> Walia: Detected Root Headers: {ROOT_HEADERS}")

def fix_includes(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    new_lines = []
    modified = False
    
    # subdir name: e.g. "core" or "db"
    abs_path = os.path.abspath(file_path)
    parent_dir = os.path.dirname(abs_path)
    dir_name = os.path.basename(parent_dir)

    # We only assume we are in a subdirectory if it matches our list
    if dir_name not in SUBDIRS:
        is_subdir = False
    else:
        is_subdir = True

    for line in lines:
        if line.strip().startswith("#include") and '"' in line:
            # Check for sibling directory includes
            # e.g. we are in 'core' and include 'db/db_shadow.h' -> should be '../db/db_shadow.h'
            
            for subdir in SUBDIRS:
                # Match "subdir/" but NOT "../subdir/"
                search_str = f'"{subdir}/'
                if search_str in line and f'"../{subdir}/' not in line:
                    if is_subdir:
                        if dir_name == subdir:
                            # Sibling include: "db/foo.h" -> "foo.h" when in "db"
                            new_line = line.replace(search_str, '"')
                            print(f"[{file_path}] [SIB] Fixed sibling {subdir} include: {line.strip()} -> {new_line.strip()}")
                        else:
                            # Cross-subdir include: "db/foo.h" -> "../db/foo.h" when in "core"
                            new_line = line.replace(search_str, f'"../{subdir}/')
                            print(f"[{file_path}] [CROSS] Fixed cross {subdir} include: {line.strip()} -> {new_line.strip()}")
                        
                        line = new_line
                        modified = True

            # Check for root headers
            # e.g. we are in 'core' and include "persistence.h" -> should be "../persistence.h"
            if is_subdir:
                for rh in ROOT_HEADERS:
                    # Match exact include "filename.h" but NOT "../filename.h"
                    if f'"{rh}"' in line and f'"../{rh}"' not in line:
                        new_line = line.replace(f'"{rh}"', f'"../{rh}"')
                        print(f"[{file_path}] [ROOT] Fixed root include: {line.strip()} -> {new_line.strip()}")
                        line = new_line
                        modified = True
                        break

        new_lines.append(line)

    if modified:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(new_lines)
            
if __name__ == "__main__":
    print(">> Walia: Scanning via Smart Patch v2.1...")
    
    # 1. Scan src/ (internal relative paths)
    for root, dirs, files in os.walk(SRC_DIR):
        for file in files:
            if file.endswith((".h", ".c")):
                fix_includes(os.path.join(root, file))
    
    # 2. Scan current (waliaos) directory (external relative paths to src)
    print(">> Walia: Scanning waliaos/ for src/ references...")
    for root, dirs, files in os.walk("."):
        # Ignore build and iso dirs
        if "build" in dirs: dirs.remove("build")
        if "iso" in dirs: dirs.remove("iso")
        
        for file in files:
            if file.endswith((".h", ".c")):
                file_path = os.path.join(root, file)
                
                # Custom logic for waliaos files: they need to reach into ../src
                with open(file_path, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
                
                modified = False
                new_lines = []
                
                # Calculate depth to project root
                # . (waliaos/) -> depth 1
                # ./core/ -> depth 2
                root_clean = os.path.normpath(root)
                if root_clean == ".":
                    depth = 1
                else:
                    depth = len(root_clean.split(os.sep)) + 1
                src_jump = "../" * depth + "src/"
                
                for line in lines:
                    if line.strip().startswith("#include") and '"' in line:
                        for subdir in SUBDIRS:
                            search_str = f'"{subdir}/'
                            if search_str in line and "../src/" not in line:
                                new_line = line.replace(search_str, f'"{src_jump}{subdir}/')
                                if new_line != line:
                                    print(f"[{file_path}] Fixed OS-to-SRC include: {line.strip()} -> {new_line.strip()}")
                                    line = new_line
                                    modified = True
                        
                        for rh in ROOT_HEADERS:
                            if f'"{rh}"' in line and "../src/" not in line:
                                new_line = line.replace(f'"{rh}"', f'"{src_jump}{rh}"')
                                if new_line != line:
                                    print(f"[{file_path}] Fixed OS-to-SRC root include: {line.strip()} -> {new_line.strip()}")
                                    line = new_line
                                    modified = True

                    new_lines.append(line)
                
                if modified:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.writelines(new_lines)

    print(">> Walia: Patch complete.")
