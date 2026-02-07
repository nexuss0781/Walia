import os

# Define the root of the source tree
SRC_DIR = "../src"

# Common patterns to fix
# Key: The incorrect include pattern
# Value: The correct relative include pattern
# This is a heuristic: if we are in src/db/, 'db/db_common.h' should be 'db_common.h'
# If we are in src/db/, 'core/common.h' should be '../core/common.h'

def fix_includes(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    new_lines = []
    modified = False
    
    # Determine the directory of the current file relative to src/
    # For example: src/db/db_shadow.h -> subdir = "db"
    abs_path = os.path.abspath(file_path)
    parent_dir = os.path.dirname(abs_path)
    dir_name = os.path.basename(parent_dir)

    for line in lines:
        if line.strip().startswith("#include"):
            # Fix db/ includes inside src/db/
            if dir_name == "db":
                if '"db/' in line:
                    # e.g. "db/db_common.h" -> "db_common.h"
                    new_line = line.replace('"db/', '"')
                    if new_line != line:
                        print(f"[{file_path}] Fixed sibling include: {line.strip()} -> {new_line.strip()}")
                        line = new_line
                        modified = True
                
                if '"core/' in line:
                    # e.g. "core/common.h" -> "../core/common.h"
                    # But ONLY if it doesn't already allow start with ../
                    if '"../core/' not in line:
                        new_line = line.replace('"core/', '"../core/')
                        if new_line != line:
                            print(f"[{file_path}] Fixed core include: {line.strip()} -> {new_line.strip()}")
                            line = new_line
                            modified = True

            # Fix core/ includes inside src/core/ (less common but possible)
            elif dir_name == "core":
                 if '"core/' in line:
                    new_line = line.replace('"core/', '"')
                    if new_line != line:
                        print(f"[{file_path}] Fixed sibling include: {line.strip()} -> {new_line.strip()}")
                        line = new_line
                        modified = True

        new_lines.append(line)

    if modified:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(new_lines)
            
if __name__ == "__main__":
    print(">> Walia: Scanning source files for include path fixes...")
    for root, dirs, files in os.walk(SRC_DIR):
        for file in files:
            if file.endswith(".h") or file.endswith(".c"):
                fix_includes(os.path.join(root, file))
    print(">> Walia: Include path patching complete.")
