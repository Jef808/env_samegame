#!/usr/bin/env python3

from pathlib import Path
import json


if __name__ == '__main__':
    path = Path.cwd() / "orig"

    for test_path in (fn for fn in path.glob("*.json")):
        test_name = test_path.stem
        out_name = test_name + ".txt"

        with open(test_path, "r") as f:
            test = json.load(f)

            with open(out_name, "w+") as out:
                out.write(test['testIn'])
