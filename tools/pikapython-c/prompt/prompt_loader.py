from pathlib import Path
from typing import Optional

PROMPT_DIR = Path(__file__).parent

def load_prompt(name: str, encoding: str = 'utf-8') -> str:
    """Load a prompt file content by name from prompt directory.

    Args:
        name: filename like 'core_task.md'
        encoding: file encoding
    Returns:
        file text
    Raises:
        FileNotFoundError if not exists
    """
    p = PROMPT_DIR / name
    if not p.is_file():
        raise FileNotFoundError(f"Prompt file not found: {p}")
    return p.read_text(encoding=encoding)
