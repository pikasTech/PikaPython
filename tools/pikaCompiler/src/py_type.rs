#[derive(Debug)]
pub struct PyType {
    type_name: String,
}

impl PyType {
    pub fn to_c_type(&self) -> String {
        if self.type_name == "int" {
            return "int".to_string();
        }
        if self.type_name == "float" {
            return "float".to_string();
        }
        if self.type_name == "pointer" {
            return "void *".to_string();
        }
        if self.type_name == "str" {
            return "char *".to_string();
        }
        if self.type_name == "" {
            return "void".to_string();
        }
        return "void *".to_string();
    }
    pub fn to_string(&self) -> String {
        return self.type_name.clone();
    }
    pub fn new(type_name: &String) -> PyType {
        return PyType {
            type_name: type_name.clone(),
        };
    }
    pub fn return_fn(&self) -> String {
        if self.type_name == "int" {
            return "method_returnInt".to_string();
        }
        if self.type_name == "float" {
            return "method_returnFloat".to_string();
        }
        if self.type_name == "pointer" {
            return "method_returnPtr".to_string();
        }
        if self.type_name == "str" {
            return "method_returnStr".to_string();
        }
        return "method_returnPtr".to_string();
    }
    pub fn set_fn(&self) -> String {
        if self.type_name == "int" {
            return "args_setInt".to_string();
        }
        if self.type_name == "float" {
            return "args_setFloat".to_string();
        }
        if self.type_name == "pointer" {
            return "args_setPtr".to_string();
        }
        if self.type_name == "str" {
            return "args_setStr".to_string();
        }
        return "args_setPtr".to_string();
    }
    pub fn get_fn(&self) -> String {
        if self.type_name == "int" {
            return "args_getInt".to_string();
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
        return "args_getPtr".to_string();
    }
}
