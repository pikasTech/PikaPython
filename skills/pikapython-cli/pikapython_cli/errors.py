class PackageError(Exception):
    def __init__(self, code, message, hint=None, stage=None, module=None):
        super().__init__(message)
        self.code = code
        self.hint = hint
        self.stage = stage
        self.module = module
