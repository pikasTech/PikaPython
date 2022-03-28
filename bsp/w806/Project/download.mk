download:
	@echo ""
	@echo "follow the prompts until the firmware download is complete:"
	@echo ""
	@../Tools/wm_tool -c $(DL_PORT) -rs at -ds 2M -dl ../Output/W806.fls