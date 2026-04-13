import os
import shutil
import subprocess
import sys


def banner():
    print(r"""
  _  __   ___   __  __    ___    _  _     ___     ____    ___
 | |/ /  | __| |  \/  |  | __|  | \| |   /   \   |__ /   |   \
 | ' <   | _|  | |\/| |  | _|   | .` |   | - |    |_ \   | |) |
 |_|\_\  |___| |_|  |_|  |___|  |_|\_|   |_|_|   |___/   |___/
                        www.kemena3d.com
 ------------------------------------------------------------------------
 Generating Kemena3D documentation with Doxygen...
 ------------------------------------------------------------------------
""")


def find_doxygen():
    found = shutil.which("doxygen")
    if found:
        return found
    windows_candidates = [
        r"C:\Program Files\doxygen\bin\doxygen.exe",
        r"C:\Program Files (x86)\doxygen\bin\doxygen.exe",
    ]
    for candidate in windows_candidates:
        if os.path.isfile(candidate):
            return candidate
    raise RuntimeError(
        "Could not find doxygen. "
        "Ensure Doxygen is installed and its bin directory is on PATH."
    )


def main():
    banner()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    doxyfile = os.path.join(script_dir, "Doxyfile")

    if not os.path.isfile(doxyfile):
        raise RuntimeError(f"Doxyfile not found at: {doxyfile}")

    doxygen = find_doxygen()
    print(f"[INFO] Using Doxygen: {doxygen}")
    print(f"[INFO] Using Doxyfile: {doxyfile}")

    print(f"[RUN] {doxygen} {doxyfile}")
    result = subprocess.run([doxygen, doxyfile], cwd=script_dir)

    if result.returncode != 0:
        raise RuntimeError("Doxygen failed.")

    print("\n------------------------------------------------------------------------")
    print("Documentation generated successfully.")
    print("------------------------------------------------------------------------")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("\n------------------------------------------------------------------------")
        print(f"Failed to generate documentation: {e}")
        print("------------------------------------------------------------------------")
        sys.exit(1)
