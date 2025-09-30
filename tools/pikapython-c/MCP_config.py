# MCP_config.py  配置文件

MODEL_NAME = "deepseek-chat"
BASE_URL = "https://api.deepseek.com/v1"
API_KEY = "sk-1cf048b09cd44573bd75950be6f6a976"
TEMPERATURE = 0
MAX_TOKENS = None
MAX_RETRYIES = 10
TIME_OUT = 180
RECURSION_LIMIT = 999  # 增大递归限制，仍需配合终止条件避免无界循环

debug_level = 1
LOG_ENABLED = True  # 日志总开关，默认开启