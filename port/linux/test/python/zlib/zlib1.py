import zlib
import json

data = {
    "name": "John Doe",
    "age": 30,
    "cars": [
        {"model": "BMW 230", "mpg": 27.5},
        {"model": "Ford Edge", "mpg": 24.1}
    ],
    "data": [
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
        [1, 231],
        [2, 232],
        [3, 233],
        [4, 234],
        [5, 235],
        [6, 236],
        [7, 237],
        [8, 238],
        [9, 239],
        [10, 240],
        [11, 241],
        [12, 242],
        [13, 243],
        [14, 244],
    ]
}

# Convert the data to a JSON string
json_str = json.dumps(data)

# Original data
original_data = json_str.encode()
# print("size of original data: ", len(original_data))

# Compress the data
compressed_data = zlib.compress(original_data, level=1)

# Decompress the data
decompressed_data = zlib.decompress(compressed_data)

# Check if the decompressed data is the same as the original data
assert decompressed_data == original_data, 'Decompressed data does not match original data'

# print("size of compressed data: ", len(compressed_data))

print("PASS")
