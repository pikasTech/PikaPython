#[derive(Debug)]
pub struct PyType {
    type_name: String,
}

impl PyType {
    pub fn to_c_type(&self) -> String {
        if self.type_name == "int" {
            return "int".to_string();
        }
        if self.type_name == "int64"{
            return "int64_t".to_string();
        }
        if self.type_name == "float" {
            return "pika_float".to_string();
        }
        if self.type_name == "pointer" {
            return "void*".to_string();
        }
        if self.type_name == "str" {
            return "char*".to_string();
        }
        if self.type_name == "bytes" {
            return "uint8_t*".to_string();
        }
        if self.type_name == "" {
            return "void".to_string();
        }
        if self.type_name == "any" {
            return "Arg*".to_string();
        }
        if self.type_name == "bool"{
            return "pika_bool".to_string();
        }
        if self.type_name == "@tupleVarPar" {
            return "PikaTuple*".to_string();
        }
        if self.type_name == "@dictVarPar" {
            return "PikaDict*".to_string();
        }
        return "PikaObj*".to_string();
    }

    pub fn to_c_type_return(&self) -> String {
        if self.type_name == "int" {
            return "int".to_string();
        }
        if self.type_name == "int64" {
            return "int64_t".to_string();
        }
        if self.type_name == "bool" {
            return "pika_bool".to_string();
        }
        if self.type_name == "float" {
            return "pika_float".to_string();
        }
        if self.type_name == "pointer" {
            return "void*".to_string();
        }
        if self.type_name == "str" {
            return "char*".to_string();
        }
        if self.type_name == "bytes" {
            return "Arg*".to_string();
        }
        if self.type_name == "" {
            return "void".to_string();
        }
        if self.type_name == "any" {
            return "Arg*".to_string();
        }
        return "PikaObj*".to_string();
    }

    // pub fn to_string(&self) -> String {
    //     return self.type_name.clone();
    // }
    pub fn new(type_name: &String) -> PyType {
        return PyType {
            type_name: type_name.clone(),
        };
    }
    pub fn return_fn(&self) -> String {
        if self.type_name == "int" {
            return "    method_returnInt(args, res);\n".to_string();
        }
        if self.type_name == "int64" {
            return "    method_returnInt(args, res);\n".to_string();
        }
        if self.type_name == "bool" {
            return "    method_returnBool(args, res);\n".to_string();
        }
        if self.type_name == "float" {
            return "    method_returnFloat(args, res);\n".to_string();
        }
        if self.type_name == "pointer" {
            return "    method_returnPtr(args, res);\n".to_string();
        }
        if self.type_name == "str" {
            return "    method_returnStr(args, res);\n".to_string();
        }
        if self.type_name == "bytes" {
            return "    method_returnArg(args, res);\n".to_string();
        }
        if self.type_name == "any" {
            return "    method_returnArg(args, res);\n".to_string();
        }
        return "    method_returnObj(args, res);\n".to_string();
    }
    // pub fn set_fn(&self) -> String {
    //     if self.type_name == "int" {
    //         return "args_setInt".to_string();
    //     }
    //     if self.type_name == "float" {
    //         return "args_setFloat".to_string();
    //     }
    //     if self.type_name == "pointer" {
    //         return "args_setPtr".to_string();
    //     }
    //     if self.type_name == "str" {
    //         return "args_setStr".to_string();
    //     }
    //     if self.type_name == "any" {
    //         return "args_setArg".to_string();
    //     }
    //     return "args_setPtr".to_string();
    // }
    pub fn get_fn(&self) -> String {
        if self.type_name == "int" {
            return "args_getInt".to_string();
        }
        if self.type_name == "int64" {
            return "args_getInt".to_string();
        }
        if self.type_name == "bool"{
            return "args_getBool".to_string();
        }
        if self.type_name == "float" {
            return "args_getFloat".to_string();
        }
        if self.type_name == "pointer" {
            return "args_getPtr".to_string();
        }
        if self.type_name == "str" {
            return "args_getStr".to_string();
        }
        if self.type_name == "bytes" {
            return "args_getBytes".to_string();
        }
        if self.type_name == "any" {
            return "args_getArg".to_string();
        }
        if self.type_name == "@tupleVarPar" {
            return "args_getTuple".to_string();
        }
        if self.type_name == "@dictVarPar" {
            return "args_getDict".to_string();
        }
        return "args_getPtr".to_string();
    }
}
