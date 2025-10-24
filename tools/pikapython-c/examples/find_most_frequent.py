# data_analysis.py

def find_most_frequent(data: list):
    """
    Finds the most frequent element in a list.
    If there is a tie, it returns the one that appeared first among the tied elements.
    Returns None if the list is empty.
    """
    if not data:
        return None

    # Step 1: Count occurrences of each item
    counts = {}
    for item in data:
        if item in counts:
            counts[item] += 1
        else:
            counts[item] = 1

    # Step 2: Find the item with the maximum count.
    # To handle ties by first appearance, we iterate through the original list.
    most_frequent_item = data[0]
    max_count = counts[most_frequent_item]

    for item in data:
        if counts[item] > max_count:
            max_count = counts[item]
            most_frequent_item = item
            
    return most_frequent_item

if __name__ == '__main__':
    # Test case 1: Simple case with numbers
    data1 = [1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5]
    print("Data:", data1)
    print("Most frequent:", find_most_frequent(data1))
    print("-" * 20)

    # Test case 2: Case with strings
    data2 = ['apple', 'banana', 'apple', 'orange', 'banana', 'apple']
    print("Data:", data2)
    print("Most frequent:", find_most_frequent(data2))
    print("-" * 20)

    # Test case 3: Tie-breaking (should return 'a' because it appears first)
    data3 = ['a', 'b', 'c', 'a', 'b', 'c', 'a', 'b']
    print("Data:", data3)
    print("Most frequent:", find_most_frequent(data3))
    print("-" * 20)
    
    # Test case 4: Empty list
    data4 = []
    print("Data:", data4)
    print("Most frequent:", find_most_frequent(data4))
    print("-" * 20)

    # Test case 5: All unique elements
    data5 = [10, 20, 30, 40, 50]
    print("Data:", data5)
    print("Most frequent:", find_most_frequent(data5))
    print("-" * 20)
