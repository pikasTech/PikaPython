## Dependency

1. W800 Serial download tool

https://www.winnermicro.com/upload/1/editor/1623809255593.rar

2. CDK IDE

https://occ.t-head.cn/community/download?id=575997419775328256

## About the project template

With reference to Keil users' habits, we have reconstructed the project template path structure to avoid too deep directories

1. Do not install CDK in Chinese directory path, and do not deploy project templates in Chinese directory path.

2. User code is placed in src and inc of Project folder

3. The compiled firmware is stored in the Output folder with the file suffix *.fls

4. There are two ways to start the project.
      a, double-click the W806_SDK.cdkws file in the root directory (recommended)
      b, CDK menu Project -> Open Multi-Project Workspace -> W806_SDK.cdkws

5. using demo code examples.
      Demo folder in the sample to replace the Project folder src and inc in the source code file can be.
