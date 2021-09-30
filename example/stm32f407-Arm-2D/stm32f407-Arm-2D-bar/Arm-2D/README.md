# README

**NOTE:** 

1. This library is a research project used to explore and demonstrate the possibilities of delivering smart-phone modern graphic user interface using low-cost and resource constraint micro-controllers. It is not a committed product from Arm, and the quality of the service is not validated with sophisticated tests but some functional tests. 
2.  The library name, i.e. **Arm-2D**, is a temporary term and might be subject to change in the future. The term "the Library" used in this documents refers to the **Arm-2D** library unless state otherwise. 



## The Purpose of Branches In This Repository

| Branch Name                               | Description                                                  | Note                                                         |
| ----------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| ***master/main***                         | The main branch of the repository. It contains the Arm-2D library and simple examples. |                                                              |
| ***main-arm-2d-developing***              | The development branch for the ***main*** branch.            | It will be merged into ***main*** branch.                    |
| ***main-arm-2d-more-examples***           | It has the same content as ***main*** branch but with additional examples. <br />The purpose of this branch is **adding more examples** for people who want to try while still **keeping the main branch simple and small**. <br />**NOTE**: This branch will not modify anything in the Arm-2D library but only focuse on examples. |                                                              |
| ***main-arm-2d-more-example-developing*** | It is the development branch for the ***main-arm-2d-more-examples-developing*** branch. | The ***main*** branch will be merged into this one which will be merged into the ***main-arm-2d-more-examples*** branch later. |



## Features of the Arm-2D Library

#### In this release ( ver0.9.8 )

The Arm-2D library provides **Low-Level 2D Image Processing Services** that are mainly used in **Display system**. The supported features include but not limited to:

- **Alpha-Blending**
  - With or without Colour-Masking
- **Image Copy / Texture Paving**
  - With or without Colour-Masking
  - Four mirroring mode: None, X-mirroring, Y-mirroring and XY-mirroring
- **Colour format conversions**
  - RGB565 and RGB888
  - Generic RGB16 and RGB32
- **Region/Window Clipping**
- **Generic Partial Frame-buffer (PFB) Support**
  - Transparent for upper layer software/GUI services
  - Easy to Use
  - No limitation on target screen resolution
  - No limitation on PFB size and shape (it could be line or cube with any size)
  - Support the Dirty Region List
  - Support swapping RGB16 high and low bytes.
  - Has built-in frame buffer pool and it is easy to support 3FB. 
  - Support User-specified buffers
- **Rotation**
  - Support for the Colour-masking by default
  - Rotation with/without alpha-blending
    - Anti-alias support is added. You can enable it by defining macro  **\_\_ARM_2D_HAS_INTERPOLATION_ROTATION\_\_** and set it to "**1**"
- **Unified and User Friendly Programmers' Mode**
  - APIs could be used in Synchronous manner (  **Classic Blocking code** ) and/or Asynchronous manner ( **Event-Driven** )
  - Support both bare-metal and RTOS
  - Ultra small memory footprint



### Planned in the Future

Following features are planned and to be introduced in the near future:

- **Alpha (bitmap) Masking schemes** 
  - New APIs will be added to Copy, Paving and Rotation.
- **Image Filters, e.g. Anti-aliasing algorithms**
- **Zooming/Stretching Algorithms**



## 1 Introduction

### 1.1 The Background

With more and more smart IoT edge devices introduced to our daily lives, people who are used to the smart-phone like a graphic user interface (GUI) want to have the same user experience when using those micro-controller-based products. This trend has been long observed and understood by Arm's leading eco-partners. 

As a result, many silicon vendors introduce dedicated hardware accelerators into their microcontroller products to help 2D image processing. Meanwhile, GUI service providers also update their product lines to target micro-controller based graphic user interface applications. Many open-source embedded GUI stacks are hot on GitHub, e.g. [LVGL](https://lvgl.io/). 

In fact, using GUI with microcontrollers isn't new at all. Still, until the recent rise of IoT and AI, people have found that the simple and monotonous graphical interface in the past is really unbearable. The pursuit of user experience similar to that of a smartphone has become a basic requirement for products. 

On the other hand, seemingly complicated graphical interfaces often only require simple texture paving. Even the so-called transparency effects are not unaffordable for microcontrollers that often run under tens of MHz or even hundreds of MHz. 

Technologies used for 2D graphics have been matured as early as the era of 8-bit Gaming Console. As 8-bit 6502 can achieve fancy graphics effects, why can't the most advanced Cortex-M processor of the day?

**Figure 1-1  2D Technologies used in Super-Mario-Brothers on NES** 

<img src="./documents/pictures/TopReadme_1_a.png" alt="image-20210318235155494" style="zoom: 50%;" /> 

### 1.2 The Problems in current solutions

As shown in **Figure 1-2**, the Linux system has a complete ecosystem from GPU drivers to GUI software services. Compared with it, the resource-constrained embedded system is obviously missing a lot in the ecosystem. To make up for this missing link, the concept of Arm-2D was introduced.

**Figure 1-2 Ecosystem Comparison between Rich Embedded and Constraint Embedded System in GUI**

<img src="./documents/pictures/TopReadme_1_2a.png" alt="image-20210318235815106" style="zoom:80%;" /> 



When we look at the conventional GUI graphics architecture in today's embedded ecosystem (as shown in **Figure 1-3** ), we will clearly observe 4 layers: the application and design layer, the GUI software service layer, the rendering layer and hardware driver layer. 

**Arm-2D focuses on accelerating the low-level 2D image processing**, and will not touch any part of the GUI software service, so the library will not compete with the GUI service providers in the Arm ecosystem. In fact, because Arm has proposed a unified set of low-level acceleration APIs, **a full ecological level of cooperation can be quickly established** between chip manufacturers that provide hardware accelerators and software providers that provide GUI services. **Everyone can concentrate on their own works**: For example, chip manufacturers can ensure that they receive a wide range of software support by adding drivers for their dedicated 2D accelerators following the Arm-2D standard, and GUI providers only need to build their GUI stack upon Arm-2D APIs; hence a wide range of device support is ensured.

**Figure 1-3 The Hierarchy of a Typical Embedded GUI System.**

<img src="./documents/pictures/TopReadme_1_2b.png" style="zoom:80%;" /> 



### 1.3 The Platform

**The library is targeting ALL Cortex-M processors with/without various hardware 2D image accelerators:**

- Armv6-M processors: Cortex-M0/M0+/M1
- Armv7-M processors: Cortex-M3/M4/M7
- Armv8-M processors: Cortex-M23/M33/M35P
- Armv8.1-M processors: Cortex-M55

**The library is designed with ACI (Arm Custom Instructions) in mind.** Specific accelerations using user-defined instructions could be easily integrated into the library without modifying upper-layer software. 

**The library is designed with various 2D image accelerator in mind.** The Support for those accelerators could be easily added into the library without modifying upper-layer software. 

**The library is designed with a deep constraint environment in mind.** **For Cortex-M processors with 4K~32K RAM that traditionally cannot afford a full-frame-buffer**, it introduces a feature called **Generic Partial Frame-buffer** which enables those existing MCUs to exchange time for space and have a fancy graphical user interface whilst still have a decent frame-rate. 



### 1.4 Dependency

- The library depends on **CMSIS 5.7.0 and above** (If you want to use Arm-2D with Cortex-M55, you need CMSIS 5.8.0 which you can get from [Github](https://github.com/ARM-software/CMSIS_5)). 
- The library is developed with the **C11** standard and depends on some **widely adopted GCC extensions**.
  - See **section 3.2** for details.
- The library should be compiled with **Arm Compiler 5**, **Arm Compiler 6**, **GCC**, **LLVM** and **IAR**
  - See **section 5** for details. 



### 1.5 Examples and Benchmark

#### 1.5.1 Example Summary

| Example              | Description                                                  | Folder                         | Note               |
| -------------------- | ------------------------------------------------------------ | ------------------------------ | ------------------ |
| Alpha-Blending       | It is an **ALL-IN-ONE** (except rotation) example that demonstrates almost all the features provided by the library. | examples/alpha_blending        | Used as benchmark. |
| Partial-Frame-buffer | **It delivers the same visual effects as Alpha-blending example but using Partial-Frame-buffer**. It can be used as a template or reference code for programmers who want to implement a graphical user interface on an MCU with small RAM. In this example, **16*16 FPB (512Bytes) is used, and the total system RAM usage is less than 2.5KByte** (including stack, heap and FPB). | examples/partial_frame_buffer. |                    |
| watch_panel          | It is a dedicated example for smart-watch like panel. For current version, it is only used to demonstrate rotation algorithms with two spinning gears. Each gear rotates at a different angular velocity and both Colour-masking and alpha-masking schemes are applied. | examples/watch_panel           | Used as benchmark  |



#### 1.5.2 Benchmark

Since there is no public benchmark available for micro-controllers, we decide to overcome this problem with the following methods and considerations:

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



**Figure 1-4 A snapshot of alpha-blending demo running on MPS3 platform**

![Alpha-blending](./documents/pictures/Alpha-blending.gif) 



- **Use the average cycle count in 1000 iterations as benchmark score.**

  - Based on that, for typical embedded application requirement, we derive a more meaningful metrics called the **Minimal Frequency Required for 30 FPS (MHz)** as shown in **Figure 1-5**. 

  

**Figure 1-5 Performance Comparison among some Cortex-M processors**

![image-20210318225839820](./documents/pictures/TopReadme_1_6_2_b.png) 



## 2 Folder Hierarchy

| Folder and File                  | Type   | Description                                                  |
| :------------------------------- | ------ | ------------------------------------------------------------ |
| Library                          | Folder | This folder contains the source files and header files of the library. |
| Documents                        | Folder | This folder contains all the documents.                      |
| Examples                         | Folder | This folder contains all the example projects.               |
| README                           | .md    | The README.md you are currently reading.                     |
| how_to_deploy_the_arm_2d_library | .md    | A step by step guidance helping you to deploy the Arm-2D library to your projects. |
| LICENSE                          |        | The Apache 2.0 License                                       |



## 3 Tips For Exploring the Library

### 3.1 "I am a library user, I only care about how to use the library"

- For library users, **ALL** useful information, i.e. type definitions, macros, prototypes of functions etc., are stored in header files which have **NO** double under-scope as their prefixes. We call those header files the **PUBLIC HEADER FILES**. 
- Please **ONLY** use APIs, macros and types that are defined in the public header files. 

**Figure 3-1 Private and Public Files** 

![image-20210317181453270](./documents/pictures/TopReadme_3_1.png) 

- Any symbol, e.g. file name, function name, macro name, type name etc., having a double under-scope as the prefix is considered as **PRIVATE** to the library. You should save your time from touching them. 

- The library is designed with the philosophy that Users are free to use anything in public header files and should not touch anything marked implicitly or explicitly as private. 

- Despite which processor you use, during the compilation, all C source files are safe to be added to the compilation (and we highly recommend you to do this for simplicity reason). For example, when you use Cortex-M4, which doesn't support Helium extension (introduced by Armv8.1-M architecture and first implemented by the Cortex-M55 processor), it is OK to include "***arm_2d_helium.c***" in the compilation process, as the C source files are constructed with environment detection pre-processing mechanisms. 

- In your application, only including "***arm_2d.h***" is sufficient to get all the services and APIs ready for you. 

- Make sure that the library is initialised by calling **arm_2d_init()** before using any of the services. 

  **NOTE**: 

  1. ***arm_2d_init()*** is a function-like macro that initialises different parts of the library depending on the feature configuration macros.  
  2. Feature configuration macros are checked by "***arm_2d_feature.h***". For the current stage of the library, please **DO NOT** override those feature configuration macros.

```c
#undef __ARM_2D_HAS_HELIUM__
#undef __ARM_2D_HAS_HELIUM_INTEGER__
#undef __ARM_2D_HAS_HELIUM_FLOAT__

#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE
#   define __ARM_2D_HAS_HELIUM__                        1
#   define __ARM_2D_HAS_HELIUM_INTEGER__                1
#   if (__ARM_FEATURE_MVE & 2)
#       define __ARM_2D_HAS_HELIUM_FLOAT__              1
#   else
#       define __ARM_2D_HAS_HELIUM_FLOAT__              0
#   endif
#else
#   define __ARM_2D_HAS_HELIUM__                        0
#   define __ARM_2D_HAS_HELIUM_INTEGER__                0
#   define __ARM_2D_HAS_HELIUM_FLOAT__                  0
#endif

#ifndef __ARM_2D_HAS_CDE__
#   define __ARM_2D_HAS_CDE__                           0
#endif

#ifndef __ARM_2D_HAS_HW_ACC__
#   define __ARM_2D_HAS_HW_ACC__                        0
#endif
#if defined(__ARM_2D_HAS_HW_ACC__) && __ARM_2D_HAS_HW_ACC__
#   if defined(__ARM_2D_HAS_ASYNC__) && !__ARM_2D_HAS_ASYNC__
#       warning As long as __ARM_2D_HAS_HW_ACC__ is set to 1,\
 __ARM_2D_HAS_ASYNC__ is forced to 1. Since you set __ARM_2D_HAS_ASYNC__ to\
 0, please remove your macro definition for __ARM_2D_HAS_ASYNC__ to avoid this\
 warning.
#   endif
#   undef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                         1
#endif

#ifndef __ARM_2D_HAS_ASYNC__
#   define __ARM_2D_HAS_ASYNC__                         1
#endif
#if defined(__ARM_2D_HAS_ASYNC__) &&  __ARM_2D_HAS_ASYNC__
#   if  !defined(__ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE) ||                        \
        __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE < 4
#       define __ARM_2D_DEFAULT_SUB_TASK_POOL_SIZE      4
#   endif
#endif

...

/*============================ MACROFIED FUNCTIONS ===========================*/

#if defined(__ARM_2D_HAS_CDE__) && !__ARM_2D_HAS_CDE__
#   define __arm_2d_cde_init()
#endif

#if defined(__ARM_2D_HAS_HELIUM__) && !__ARM_2D_HAS_HELIUM__
#   define __arm_2d_helium_init()
#endif

#if defined(__ARM_2D_HAS_HW_ACC__) && !__ARM_2D_HAS_HW_ACC__
#   define __arm_2d_acc_init()
#endif

#if defined(__ARM_2D_HAS_ASYNC__) && !__ARM_2D_HAS_ASYNC__
#   define __arm_2d_async_init()
#endif


#undef arm_2d_init
#define arm_2d_init()                                                           \
        do {                                                                    \
            __arm_2d_init();                                                    \
            __arm_2d_async_init();                                              \
            __arm_2d_helium_init();                                             \
            __arm_2d_cde_init();                                                \
            __arm_2d_acc_init();                                                \
        } while(0)
...
```



### 3.2 "I am interested in the implementation"

- We apologise that at the current stage (it's the early stage, as you can see), there is no sufficient guidance or documents about:
  - How the library is implemented
  - How to contribute
  - How to add new features
  - What's the design principles behind the code
  - What's the structure of the design in details
- Some design considerations:
  - The library supports **Arm Compiler 5/6**, **GCC**, **LLVM** and **IAR**.
  - The library supports **ALL** Cortex-M processors. There should be no problem for working with existing Cortex-M processors, i.e. **Cortex-M0/M0+/M1/M3/M4/M7/M23/M33/M35P/M55**. If you find any issue, please feel free to let us know. 
  - The library is designed with some **OOPC** (Object-Oriented Programming with ANSI-C) methodologies. And the bottom line is that any methods and tricks adopted in this library should come with no or very little cost. 
- This library is compliant with **C11** standard and uses some **widely accepted GCC extensions**:
  - [Macros with a Variable Number of Arguments](https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html#Variadic-Macros) 
  - [ ***\_\_alignof\_\_()*** ](https://gcc.gnu.org/onlinedocs/gcc/Alignment.html#Alignment) 
  - [Unnamed Structure and Union Fields](https://gcc.gnu.org/onlinedocs/gcc/Unnamed-Fields.html)
- Some of the definitions are written with the support of the **Microsoft Extensions** in mind \( ***-fms-extensions*** \), but **the library never depends on it**. This means that if programmers enable the support of the Microsoft Extensions in their project, they can benefit from it. 
- This library follows "***Using Extensions to replace Modifications***" principle
  - Keywords ***\_\_WEAK*** and ***\_\_OVERRIDE\_WEAK*** are introduced for default functions and extensions; it is similar to the concept of "virtual functions" and "override functions" in C#. 
    - ***arm_2d_async.c*** is used to override some infrastructure functions in ***arm_2d.c*** to support asynchronous mode in the programmers' mode.  
    - ***arm_2d_helium.c*** is used to override some default software algorithm implementations across the library. 
  - Supports for hardware accelerators (both from Arm and 3rd-parties) should be added in the same manner in the future. 
    
    - Override the target low level IO defined with ***def_low_lv_io()*** macro that originally defined in ***arm_2d_op_table.c*** to add your own version of algorithms and hardware accelerations. For example, if you want to add alpha-blending support for RGB565 using your 2D hardware accelerator, you should do the following steps:
    
      1. In one of your own C source code, override the definition of ***\_\_ARM_2D_IO_ALPHA_BLENDING_RGB565***
    
         ```c
         //! PLEASE add following three lines in your hardware adapter source code
         #define __ARM_2D_IMPL__
         #include "arm_2d.h"
         #include "__arm_2d_impl.h"
         
         ...
         
         __OVERRIDE_WEAK
         def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB565, 
                         __arm_2d_rgb565_sw_alpha_blending,
                         __arm_2d_rgb565_my_hw_alpha_blending);
         ```
    
      2. Copy the function body of ***\_\_arm\_2d\_rgb565\_sw\_alpha\_blending()*** to your source code as a template of the ***hardware adaptor*** and rename it as ***\_\_arm_2d_rgb565_my_hw_alpha_blending()***
    
      3. Modify ***\_\_arm_2d_rgb565_my_hw_alpha_blending()*** to use your own hardware accelerator. 
    
      4. Based on the arguments passed to the function and the capability of your 2D accelerator, you can:
    
         - return "***ARM_2D_ERR_NOT_SUPPORT***" if the hardware isn't capable to do what is requested.
         - return "***arm_fsm_rt_cpl***" if the task is done immediately and no need to wait.
         - return "***arm_fsm_rt_async***" if the task is done asynchronously and later report to arm-2d by calling function ***__arm_2d_notify_sub_task_cpl()***. 
    
      ***NOTE***: The Arm-2D pipeline will keep issuing tasks to your ***hardware adaptor***, please quickly check whether the hardware is capable of doing the work or not, and then add the task (an ***__arm_2d_sub_task_t*** object) to a list in ***First-In-First-Out*** manner if your hardware adaptor decides to keep it. After that, your hardware accelerator can fetch tasks one by one.  

``````c
typedef struct __arm_2d_sub_task_t __arm_2d_sub_task_t;


typedef arm_fsm_rt_t __arm_2d_io_func_t(__arm_2d_sub_task_t *ptTask);

typedef struct __arm_2d_low_level_io_t {
    __arm_2d_io_func_t *SW;
    __arm_2d_io_func_t *HW;
} __arm_2d_low_level_io_t;

...

/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB16, __arm_2d_rgb16_sw_tile_copy);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_RGB32, __arm_2d_rgb32_sw_tile_copy);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_RGB16, __arm_2d_rgb16_sw_tile_fill);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_RGB32, __arm_2d_rgb32_sw_tile_fill);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB16, 
                __arm_2d_rgb16_sw_tile_copy_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_COLOUR_MASKING_RGB32, 
                __arm_2d_rgb32_sw_tile_copy_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB16, 
                __arm_2d_rgb16_sw_tile_fill_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_COLOUR_MASKING_RGB32, 
                __arm_2d_rgb32_sw_tile_fill_with_colour_masking);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_RGB16, __arm_2d_rgb16_sw_colour_filling);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_RGB32, __arm_2d_rgb32_sw_colour_filling);

__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB565, 
                __arm_2d_rgb565_sw_alpha_blending);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_RGB888, 
                __arm_2d_rgb888_sw_alpha_blending);

__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB565, 
                __arm_2d_rgb565_sw_alpha_blending_with_colour_masking);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_BLENDING_WITH_COLOUR_MASKING_RGB888, 
                __arm_2d_rgb888_sw_alpha_blending_with_colour_masking);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_alpha);
__WEAK
def_low_lv_io(__ARM_2D_IO_ALPHA_FILL_COLOUR_RGB888, 
                __arm_2d_rgb888_sw_colour_filling_with_alpha);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_DRAW_POINT, __arm_2d_sw_draw_point);

__WEAK
def_low_lv_io(__ARM_2D_IO_DRAW_PATTERN_RGB16, __arm_2d_rgb16_sw_draw_pattern);
__WEAK
def_low_lv_io(__ARM_2D_IO_DRAW_PATTERN_RGB32, __arm_2d_rgb32_sw_draw_pattern);

__WEAK
def_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB565, 
                __arm_2d_sw_convert_colour_to_rgb565);
__WEAK
def_low_lv_io(__ARM_2D_IO_COLOUR_CONVERT_TO_RGB888, 
                __arm_2d_sw_convert_colour_to_rgb888);
                
__WEAK
def_low_lv_io(__ARM_2D_IO_ROTATE_RGB565, 
                __arm_2d_rgb565_sw_rotate);

__WEAK
def_low_lv_io(__ARM_2D_IO_ROTATE_RGB888, 
                __arm_2d_rgb888_sw_rotate);
``````



## 4 Documentation

| Name                                                         | Description                                                  | Location  |
| ------------------------------------------------------------ | ------------------------------------------------------------ | --------- |
| **README.md**                                                | It is the document that you are reading. It provides basic information and guidance for the arm-2d library. | (root)    |
| [**how_to_deploy_the_arm_2d_library.md**](./how_to_deploy_the_arm_2d_library.md) | A step by step guide that helps you to deploy the library to your existing or new projects. | (root)    |
| **[introduction.md](./documents/Introduction.md)**           | A relatively detailed introduction for the library, including basic concepts, programmers' mode etc. | documents |
| **[how_to_use_tile_operations.md](./documents/how_to_use_tile_operations.md)** | A detailed document elaborates the APIs dedicated to basic tile operations in the arm-2d library. | documents |
| **how_to_use_alpha_blending_operations.md**                  | A detailed document elaborates the APIs dedicated to alpha-blending services provided by the arm-2d library. | documents |
| **how_to_use_conversion_operations.md**                      | A detailed document elaborates the APIs dedicated to colour space conversion services provided by the arm-2d library. | documents |
| **how_to_use_drawing_operations.md**                         | A detailed document elaborates the APIs that provide basic point-drawing and colour-filling services in the arm-2d library. | documents |



## 5 Limitations

### 5.1 The Generic Limitations

- The library focus on Cortex-M processors in principle.
- The library should be compiled with the following compilers:
  - Arm Compiler 5
  - Arm Compiler 6
  - GCC
  - LLVM
  - IAR
- The library focus on **Low Level Pixel Processing Acceleration**
  - In principle, the library will **NOT** provide APIs for content creation, such as drawing shapes, text display etc., but simple point drawing APIs.
  - In principle, the library will **NOT** provide data structures or related algorithms essential for creating a GUI, for example, element tree, GUI message handling and the tree traversal algorithms.



### 5.2 The Temporary Limitations

- The library currently is developed and validated using Arm Compiler 6, Arm Compiler 5 and GCC.
- The library currently can only be used in the C environment. C++ support will be added later.
- The library currently only supports two specific colours, i.e. **RGB565** and **RGB888**
- Anti-aliasing algorithms haven't been implemented yet.
- Zooming (Stretching) algorithms haven't been implemented yet.
- Alpha-masking algorithms haven't been implemented yet.
- The library currently only provides default software algorithms and a **[Helium](https://developer.arm.com/architectures/instruction-sets/simd-isas/helium) based acceleration library**. 
  - Although planned, no hardware acceleration is implemented or supported for now.
  - Although planned and implemented, the [ACI (Arm Custom Instruction)](https://developer.arm.com/architectures/instruction-sets/custom-instructions) acceleration solutions are not open-source for now. Please contact local Arm FAE for details. 
- The provided example projects only run on [MPS2](https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/mps2), [MPS3](https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/mps3), [FVP](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/cortex-m-platforms-software) and some 3rd party development platforms, e.g. STM32F746G-Discovery. 
  - Feel free to try the library on your own devices. The library depends on No specific peripheral. 
- Example projects are based on MDK (one of the most popular development environments for Cortex-M processors ).



## 6 Feedback

As mentioned at the beginning, the purpose of this project is to explore and demonstrate the possibilities of delivering smart-phone modern graphic user interface using low-cost and resource constraint micro-controllers. We expect that this arm-2d library could inspire more similar initiatives and engineering practices. Hence, your feedback and thoughts are precious to us. On the other hand, although this is a research project, with your help and support, it can be promoted as a formal product similar to other open-source projects delivered by Arm. 

If you want to spend some time to try the library and leave some thoughts, please feel free to raise issues and kindly provide us with some critical information such as:

- The target application and industry segment which you want to introduce a GUI using Cortex-M processors
- The constraint of your platform, such as the size of the RAM, ROM, system frequency, the average power consumption etc.
- The LCD resolution and target frame-rate (FPS)
- Algorithms that you like and don't like
- Algorithms that are missing in the library and why you recommend them.
- What kind of device do you use? 
  - Does it contain HW accelerators for 2D image processing? 
  - What kind of features/functionalities does the accelerator provide?
  - Does the HW accelerator contain features that arm-2d currently doesn't support?
- Any other thoughts or suggestions.



Thank you for your time.

***Arm-2D Development Team.***