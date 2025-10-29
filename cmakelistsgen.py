import os, pathlib

root = pathlib.Path(".").resolve()
exts_src = {".c", ".cc", ".cpp", ".cxx"}
exts_hdr = {".h", ".hh", ".hpp", ".hxx", ".inl"}
files = []

for p in sorted(root.rglob("*")):
    if p.is_file():
        e = p.suffix.lower()
        if e in exts_src or e in exts_hdr:
            files.append(str(p.relative_to(root)).replace("\\", "/"))

print("set(PROJECT_FILES")
for f in files:
    print(f"    %s" % f)
print(")")
