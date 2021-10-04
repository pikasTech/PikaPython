class PyObj(BaseObj):

    def importClass(className: str, fp: str):
        pass

    def newObj(objName: str, className: str, fp: str):
        pass

    def makeInlcude(fp: str):
        pass

    def makeNewObj(fp: str):
        pass

    def getInclude() -> str:
        pass


class PyMethod (SysObj):
    def makeMethodFun(fp: str):
        pass

    def makeMethodDeclear(fp: str):
        pass

    def makeMethodDefine(fp: str):
        pass

class PyClass(SysObj):
    obj = PyObj()
    PyMethod() 
    def setSuper(superClassName: str):
        pass

    def makeApi(path: str):
        pass

    def makeHead(path: str):
        pass
