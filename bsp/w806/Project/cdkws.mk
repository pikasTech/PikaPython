.PHONY: clean All Project_Title Project_Build Project_PostBuild

All: Project_Title Project_Build Project_PostBuild

Project_Title:
	@echo "----------Building project:[ W806_SDK - BuildSet ]----------"

Project_Build:
	@make -r -f W806_SDK.mk -j 8 -C  ./ 

Project_PostBuild:
	@echo Executing Post Build commands ...
	@export CDKPath="D:/C-Sky/CDK" CDK_VERSION="V2.10.5" ProjectPath="D:/projects_1/display_and_control/WM_SDK_W806_0.4.0_FreeRTOS/WM_SDK_W806_0.4.0_FreeRTOS/Project/" && D:/projects_1/display_and_control/WM_SDK_W806_0.4.0_FreeRTOS/WM_SDK_W806_0.4.0_FreeRTOS/Project/cdk_aft_build.sh;D:/projects_1/display_and_control/WM_SDK_W806_0.4.0_FreeRTOS/WM_SDK_W806_0.4.0_FreeRTOS/Project/aft_build_project.sh 
	@echo Done


clean:
	@echo "----------Cleaning project:[ W806_SDK - BuildSet ]----------"

