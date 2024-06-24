class kvdb_t:
    ...


class KVDB_CTRL:
    SET_SEC_SIZE: int
    GET_SEC_SIZE: int
    SET_LOCK: int
    SET_UNLOCK: int
    SET_FILE_MODE: int
    SET_MAX_SIZE: int
    SET_NOT_FORMAT: int
    def __init__(self): ...


class KVDB:

    def __init__(self, name: str, path: str, default_kv: dict,
                 user_data: any): ...

    def control(self, cmd: int, arg: any) -> int: ...

    def deinit(self): ...

    def set_blob(self, key: str, blob: any) -> int: ...

    def get_blob(self, key: str, size: int) -> list: ...

    def print(self): ...

    def __del__(self): ...
