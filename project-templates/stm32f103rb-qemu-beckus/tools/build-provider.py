#!/usr/bin/env python3
import subprocess
import sys

from provider import IMAGE, ProviderError, template_root, validate_provider


def main():
    root = template_root()
    provider = root / "provider"
    command = ["docker", "build", "--tag", IMAGE, str(provider)]
    print("Building Beckus QEMU provider: %s" % IMAGE, flush=True)
    result = subprocess.run(command, cwd=root, check=False)
    if result.returncode != 0:
        print(
            "Beckus QEMU provider build failed with status %d"
            % result.returncode,
            file=sys.stderr,
        )
        return result.returncode
    try:
        validate_provider()
    except ProviderError as error:
        print("Provider validation failed: %s" % error, file=sys.stderr)
        return 1
    print("Beckus QEMU provider is ready: %s" % IMAGE)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
