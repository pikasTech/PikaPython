# README



## 1. Default Examples 



### 1.1 Alpha-blending

**PROJECT:    alpha-blending**

It is an **ALL-IN-ONE** example that demonstrates almost all the features provided by the library. It is also used as a benchmark of Arm-2D. Since there is no public benchmark available for micro-controllers, we decide to overcome this problem with the following methods and considerations:

- **Choose the widely used algorithms in embedded GUI as the body of the benchmark**
  - Alpha-blending
  - Image Copy
  - Texture Paving
- **Simulate a typical application scenario with sufficient complexity**
  - Background with Texture paving (switching different mirroring modes every 4 second)
  - Foreground picture 
  - Two constructed layers for alpha-blending and texture paving
  - Moving icons
  - Spinning busy wheel
- **Choose a typical low-cost LCD resolution 320*240 in RGB565**
- **Let those layers float with different angles and speed to cover a sufficient number of conditions.**
- **Record the cycle count used for blending one frame and run 1000 iterations (frames).** 



**Figure 1-1 A snapshot of alpha-blending demos running on MPS3 platform**

![Alpha-blending](../documents/pictures/Alpha-blending.gif)  



- **Use the average cycle count in 1000 iterations as benchmark score.**

  - Based on that, for typical embedded application requirement, we derive a more meaningful metrics called the **Minimal Frequency Required for 30 FPS (MHz)** as shown in **Figure 1-5**. 

  

**Figure 1-2 Performance Comparison among some Cortex-M processors**

![image-20210318225839820](../documents/pictures/TopReadme_1_6_2_b.png)  



### 1.2 Partial-Frame-Buffer

**PROJECT:    partial_frame_buffer**

The visual effects is the same as Alpha-blending but using Partial-Frame-buffer scheme.



### 1.3 Watch-Panel

**PROJECT:    watch_panel**

It is an example of rotation APIs. It consists of five layers:

1. A floating background layer (Using Tile Copy)
2. A clock_panel layer (Using Alpha-Blending-With-Colour-Masking)
3. A small gold gear (Rotation)
4. A big black gear (Rotation with Alpha Blending)
5. A red pointer (Rotation)

This demo also shows how a fancy looking clock could be rendered with just less than 10K RAM (or even less by using a smaller PFB size). 

![watch_panel](../documents/pictures/watch_panel.gif) 