# Header Files Summary {#headers}

## 1 Overview

As an open-source project, arm-2d serves users with different purposes and background. In general, arm-2d is designed with two groups of users in mind:

- **Library Users** - who use arm-2d public APIs in their applications
- **Professional Users** - all other users who are
  - Integrating arm-2d with GUI stacks
  - Accelerating arm-2d with some 2D HW accelerators
  - Hobbyists writing new GUIs and/or 2D game engines based on arm-2d.

For the Library Users, **Public Header Files** provide all the information required to use arm-2d services. For the Professional Users, additional information are provided in **Private Header Files** to help their design. 



## 2 Public Header Files

In the `Library/Include` folder, all header files (*.h) **without** double-underscore-prefix, i.e. "__", are considered as **PUBLIC HEADER FILES**. They are listed in the **Table 2-1**.

**Table 2-1 Summary of Public Header Files**

| File Name                     | Description                                                  | Note                                                 |
| ----------------------------- | ------------------------------------------------------------ | ---------------------------------------------------- |
| ***arm_2d.h***                | **The main entry for all users.** To use any arm-2d services, you **must** include this header file first. It includes all other public header files list below. |                                                      |
| ***arm_2d_types.h***           | This header file provides the definitions for the common and/or important data types, enumerations etc. |                                                      |
| ***arm_2d_utils.h***          | This header file provides utilities used in arm-2d, such as macros for compiler-detection, OOPC, C language helpers etc. |                                                      |
| ***arm_2d_features.h***       | This header file provides feature detection services for compiler and target processors. It also help to define, detect and validate macro-based-options for switching arm-2d features on  and off. | **Do Not Modify**                                    |
| ***arm_2d_op.h***             | This header file list all the arm-2d OPCODE, i.e. ***ARM_2D_OP_xxxx*** |                                                      |
| ***arm_2d_tile.h***           | The header file for all basic tile operations, e.g. tile-copy/filling with/without mirroring, with/without colour-keying etc. | [Doc](../../documentation/how_to_use_tile_operations.md) |
| ***arm_2d_draw.h***           | The header file for all drawing related operations, e.g. filling rectangular area, drawing points, drawing bit-patterns etc. |                                                      |
| ***arm_2d_conversion.h***     | The header file for colour-format conversion operations, e.g. conversion between RGB565 and RGB888 etc. |                                                      |
| ***arm_2d_alpha_blending.h*** | The header file for alpha-blending centric operations, e.g. alpha-blending, copy with masks, colour-keying with opacity etc. |                                                      |
| ***arm_2d_transform.h***      | The header file for transform operations, i.e. rotation and/or scaling (zooming). |                                                      |



## 3 Private Header Files

In the `Library/Include` folder, all header files (*.h) **with** double-underscore-prefix, i.e. "__", are considered as **PRIVATE HEADER FILES**. Only professional users should read them.

**Table 3-1 Summary of Private Header Files**

| File Name                      | Description                                                  | Note                                                         |
| ------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| ***__arm_2d_impl.h***          | **The main entry for all professional users.** For advanced development (not for normal applications), you must include this header file that includes most of the private header files list below. |                                                              |
| ***__arm_2d_direct.h***        | This file lists all the low level implementations for specific variants of 2D operations. It is useful when integrating arm-2d with a GUI stack. | This file is included by ***__arm_2d_impl.h***, please do NOT include it directly. |
| ***__arm_2d_math.h***          | This file provides some math related utilities.              |                                                              |
| ***__arm_2d_math_helium.h***   | This file provides some math related utilities for helium only. It is **NOT** intended to be used outside of arm-2d. | **Do NOT Use.**                                              |
| ***__arm_2d_paving.h***        | This file lists some internal macro templates. It is **NOT** intended to be used outside of arm-2d. | **Do NOT Use.**                                              |
| ***__arm_2d_paving_helium.h*** | This file lists some internal macro templates for helium only. It is **NOT** intended to be used outside of arm-2d. | **Do NOT Use.**                                              |
| ***__arm_2d_utils_helium.h***  | This file lists some internal utils for helium only. It is **NOT** intended to be used outside of arm-2d. | **Do NOT Use.**                                              |



## 4 Templates

In the `Library/Include/template` folder, there are some header files used as templates for purposes including but not limited to configuration etc. 

**Table 4-1 Summary of Templates**

| File Name          | Description                                             | Note |
| ------------------ | ------------------------------------------------------- | ---- |
| ***arm_2d_cfg.h*** | A configuration template used in the arm-2d cmsis-pack. |      |
