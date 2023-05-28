use crate::py_type::PyType;

#[derive(Debug)]
pub struct PyArg {
    py_type: PyType,
    name: String,
}

impl PyArg {
    pub fn new(name: &String, type_name: &String) -> PyArg {
        let py_arg = PyArg {
            name: name.clone(),
            py_type: PyType::new(type_name),
        };
        return py_arg;
    }
    pub fn name(&self) -> String {
        return self.name.clone();
    }
    pub fn c_type(&self) -> String {
        return self.py_type.to_c_type();
    }
    pub fn get_local_arg(&self) -> String {
        return format!(
            "    {} {} = {}(args, \"{}\");\n",
            self.c_type(),
            self.name(),
            self.py_type.get_fn(),
            self.name()
        );
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_get_arg_to_local() {
        let arg = PyArg::new(&"arg".to_string(), &"str".to_string());
        assert_eq!(
            arg.get_local_arg(),
            "    char * arg = args_getStr(args, \"arg\");\n"
        );
    }
}
