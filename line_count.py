import os

def get_lines_and_characters(directory, extensions=None):
    total_files = 0
    total_lines = 0
    total_characters = 0

    for root, _, files in os.walk(directory):
        for file in files:
            if extensions and not file.endswith(tuple(extensions)):
                continue
            file_path = os.path.join(root, file)
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
                    total_files += 1
                    total_lines += len(lines)
                    total_characters += sum(len(line) for line in lines)
            except (UnicodeDecodeError, FileNotFoundError) as e:
                print(f"Skipping {file_path}: {e}")

    return total_files, total_lines, total_characters

# Directory of your source code
source_directory = "src"

# Specify file extensions to include (e.g., '.cpp', '.h', etc.)
file_extensions = ['.cpp', '.h', '.inl']

files, lines, characters = get_lines_and_characters(source_directory, file_extensions)

print(f"Total files: {files}")
print(f"Total lines: {lines}")
print(f"Total characters: {characters}")
os.system("pause")
