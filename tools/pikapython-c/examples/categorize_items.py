def categorize_items(items: list) -> dict:
    """
    Categorizes items from a list into a dictionary.

    This function takes a list of mixed integers and strings and returns a
    dictionary containing three keys:
    - 'integers': A list of all integer items.
    - 'strings': A list of all string items.
    - 'counts': A dictionary where keys are string representations of
                items (e.g., 'i_123' for int, 's_apple' for str) and
                values are their frequencies.

    Args:
        items: A list containing a mix of integers and strings.

    Returns:
        A dictionary with categorized lists and item counts.
        Returns None if the input list is empty.
    """
    if not items:
        return None

    integers = []
    strings = []
    counts = {}

    for item in items:
        if isinstance(item, int):
            integers.append(item)
            key = 'i_{}'.format(item)
            counts[key] = counts.get(key, 0) + 1
        elif isinstance(item, str):
            strings.append(item)
            key = 's_{}'.format(item)
            counts[key] = counts.get(key, 0) + 1

    return {
        'integers': integers,
        'strings': strings,
        'counts': counts
    }

# Example usage (for reference, not part of the agent's task)
if __name__ == '__main__':
    data = [1, "apple", 2, "banana", 1, "apple", 3, "cherry"]
    result = categorize_items(data)
    print("Categorized Items:", result)

    # Expected output structure:
    # {
    #     'integers': [1, 2, 1, 3],
    #     'strings': ['apple', 'banana', 'apple', 'cherry'],
    #     'counts': {'i_1': 2, 's_apple': 2, 'i_2': 1, 's_banana': 1, 'i_3': 1, 's_cherry': 1}
    # }
