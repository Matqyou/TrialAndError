import re, pathlib

root = pathlib.Path("src").resolve()
code_exts = {".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".hh", ".hxx", ".inl"}

include_re = re.compile(r'(#\s*include\s*)([<"])([^">]+)([">])')

count_changed = 0

for f in root.rglob("*"):
    if f.suffix.lower() not in code_exts:
        continue

    text = f.read_text(encoding="utf-8", errors="ignore")
    new_text = text

    for match in include_re.finditer(text):
        inc = match.group(3)

        # skip system / SDL includes
        if inc.startswith("SDL") or inc.startswith("SDL2/") or not any(ch in inc for ch in ("/", "\\")):
            continue

        # resolve absolute path
        inc_path = (f.parent / inc).resolve()
        try:
            rel = inc_path.relative_to(root.parent)  # from project root
        except ValueError:
            continue

        new_inc = f"<{str(rel).replace('\\', '/')}>"
        new_text = new_text.replace(match.group(0), f'#include {new_inc}')

    if new_text != text:
        count_changed += 1
        f.write_text(new_text, encoding="utf-8")
        print("Updated", f.relative_to(root.parent))

print(f"✅ Done. Updated includes in {count_changed} files.")
