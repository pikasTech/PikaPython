# How to Use

image to C array converter with optional resize

## Usage :  

img2c.py <***image***> <***output***> <***array_name***> [resizeX] [resizeY]



| Arguments                        | Description                                                  | Memo     |
| -------------------------------- | ------------------------------------------------------------ | -------- |
| **\<image\>**                    | image file (png, bmp, etc..)                                 |          |
| **\<output\>**                   | C file containing 2 arrays with RGB56 and alpha values and arm-2d related structure |          |
| **\<array_name\>**               | arrays prefix                                                |          |
| **\[resizeX\]**  **\[resizeY\]** | optional resized dimensions                                  | Optional |



## Example:

```
python img2c.py banana.png array.c BananaSmall 20 20
```



## Installation:

Needs ***python3***, ***pillow*** (https://python-pillow.org/) & ***numpy***

```
pip install Pillow
pip install numpy 
```

